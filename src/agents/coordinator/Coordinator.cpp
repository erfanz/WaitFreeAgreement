///*
// *  Coordinator.cpp
// *
// *  Author		: Erfan Zamanian
// *  Created on	: Jun 10, 2015
// *  Description : PLEASE FILL IN THIS PART
// *
// */

#include "Coordinator.hpp"
#include "../../base_types/HashMaker.hpp"
#include "../../request_buffer/request/BucketReadRequest.hpp"
#include "../../request_buffer/request/CASRequest.hpp"
#include "../../request_buffer/request/EntryReadRequest.hpp"
#include "../../request_buffer/request/EntryWriteRequest.hpp"
#include "../../request_buffer/request/StateReadRequest.hpp"
#include "../../request_buffer/request/StateWriteRequest.hpp"
#include <future>         // std::promise, std::future
#include <unistd.h>


#define CLASS_NAME	"Coord"


Coordinator::Coordinator(const primitive::coordinator_num_t coordNum, std::shared_ptr<RequestBuffer> requestBuffer)
: coordinatorID_(coordNum),
  requestBuffer_(requestBuffer){
	generationNum_ = 0;
	freeBufferOffset_ = (uint32_t)0;
	localMSIndex_ = coordinatorID_;
	DEBUG_COUT(CLASS_NAME, __func__, "Coordinator created with ID " << (int)coordinatorID_);
}

primitive::coordinator_num_t Coordinator::getID() {
	return coordinatorID_;
}

Coordinator::~Coordinator() {
	DEBUG_COUT(CLASS_NAME, __func__, "Coordinator " << (int)coordinatorID_ << " destroyed ");
}

ErrorType Coordinator::applyChange(const std::vector<Dependency> &deps, const std::vector<KeyValue> &updates, const TID tID, LogEntry **newEntry){
	DEBUG_COUT(CLASS_NAME, __func__, "Applying Change: " << printChange(deps, updates) << " by coordinator " << (int)coordinatorID_);

	ErrorType eType;

	if (updates.size() == 0) {
		DEBUG_COUT(CLASS_NAME, __func__, "Change does not contain any update");
		return error::NO_UPDATE_KEY_IN_CHANGE;
	}

	(void)tID;	// TODO: since it's unused

	Pointer *entryPointer = NULL;

	if ((eType = createNewPointer(deps, updates, &entryPointer)) != error::SUCCESS)
		return eType;

	if ((eType = makeNewLogEntry(deps, updates, *entryPointer, newEntry ) ) != error::SUCCESS)
		return eType;

	if ((eType = propagateLogEntry(**newEntry)) != error::SUCCESS)
		return eType;

	if ((eType = publishChanges(**newEntry)) != error::SUCCESS)
		// TODO: we should call resolve
		return error::CHANGE_FAILURE;
	else {
		// All the CASes succeeded
		replicateState(**newEntry, EntryState::SERIALIZED);
		DEBUG_COUT(CLASS_NAME, __func__, "Change successfully applied by coordinator " << (int)coordinatorID_);
		return error::SUCCESS;
	}
}

ErrorType Coordinator::createNewPointer(const std::vector<Dependency> &deps, const std::vector<KeyValue> &updates, Pointer **pointer) {
	primitive::entry_size_t entrySize = LogEntry::calculateEntrySize(deps, updates);
	*pointer = new Pointer(coordinatorID_, generationNum_, entrySize, freeBufferOffset_);

	freeBufferOffset_ += entrySize;	// moving forward the free buffer head

	DEBUG_COUT(CLASS_NAME, __func__, "Pointer created (" << (*pointer)->toHexString()
			<< ") by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::makeNewLogEntry(const std::vector<Dependency> &deps, const std::vector<KeyValue> &updates, const Pointer &entryPointer, LogEntry **entry __attribute__((unused))) const{
	*entry = new LogEntry(deps, updates, entryPointer, EntryState::UNKNOWN);
	DEBUG_COUT(CLASS_NAME, __func__, "Log entry created with pointer = "
			<< (*entry)->getCurrentP().toHexString() << " by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::propagateLogEntry(const LogEntry &entry){
	std::vector<std::promise<ErrorType> > errorPromises(config::MEMORY_SERVER_CNT);
	std::vector<std::future<ErrorType> > errorFutures(config::MEMORY_SERVER_CNT);
	ErrorType eType;

	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
		errorFutures[i] = errorPromises[i].get_future();

		std::shared_ptr<Request> reqPtr (new EntryWriteRequest(
					errorPromises[i],
					i,
					coordinatorID_,
					entry,
					entry.getCurrentP().getOffset(),
					entry.getCurrentP().getLength() ) );

		requestBuffer_->add(reqPtr);
	}

	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
		eType = errorFutures[i].get();
		if (eType != error::SUCCESS)
			return eType;
	}

	DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString()
			<< " propagated to all replicas by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::publishChanges(const LogEntry &entry){
	ErrorType eType;
	std::vector<Dependency> dependencies = entry.getDependencies();
	size_t n = dependencies.size();

	std::vector<std::promise<ErrorType> >	errorPromises(config::MEMORY_SERVER_CNT * n);
	std::vector<std::future<ErrorType> >	errorFutures (config::MEMORY_SERVER_CNT * n);

	std::vector<Pointer> actualHeads(config::MEMORY_SERVER_CNT * n);

	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
		for (size_t d = 0; d < n; d++){
			errorFutures[(i * n) + d] = errorPromises[(i * n) + d].get_future();

			// std::string s = "ms, bucket: ( " + utilities::ToString<int>(i) + "," + utilities::ToString<int>(dependencies.at(d).getBucketID()) + ") expected: " + dependencies.at(d).getPointer().toHexString();
			// std::cout << s << std::endl;

			std::shared_ptr<Request> reqPtr (new CASRequest(
					errorPromises[(i * n) + d],
					i,
					dependencies.at(d).getPointer(),
					entry.getCurrentP(),
					(primitive::offset_t)dependencies.at(d).getBucketID(),
					actualHeads[(i * n) + d] ) );

			requestBuffer_->add(reqPtr);
			usleep(1);
		}
	}

	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
		for (size_t d = 0; d < n; d++){
			eType		= errorFutures[(i * n) + d].get();
			if (eType != error::SUCCESS) {
				DEBUG_CERR(CLASS_NAME, __func__, "Error " << eType << ": CAS failed for dependency "
						<< dependencies.at(d).getBucketID() << " (" << dependencies.at(d).getPointer().toHexString() << ") on memory server context "<< i);
				return eType;
			}
		}
		DEBUG_COUT(CLASS_NAME, __func__, "CAS " << n << " hash bucket(s) on memory server "
				<< i << " by coordinator " << (int)coordinatorID_);

	}
	DEBUG_COUT(CLASS_NAME, __func__, "Published the change by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::replicateState(const LogEntry &entry, const EntryState::State state) {
	std::vector<std::promise<ErrorType> >	errorPromises(config::MEMORY_SERVER_CNT);
	std::vector<std::future<ErrorType> >	errorFutures (config::MEMORY_SERVER_CNT);
	ErrorType eType;

	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
		errorFutures[i ] = errorPromises[i].get_future();

		std::shared_ptr<Request> reqPtr (new StateWriteRequest(
				errorPromises[i],
				i,
				entry.getCurrentP(),
				state) );

		requestBuffer_->add(reqPtr);
	}

	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
		eType	= errorFutures[i].get();
		if (eType != error::SUCCESS)
			return eType;
		else
			DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString()
					<< " marked " << EntryState::getStateName[state] << " on log journal[" << (int)coordinatorID_ << "][" << i << "]");
	}

	DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString()
			<< " marked " << EntryState::getStateName[state] << " on all replicas by coordinator " << (int)coordinatorID_);

	return error::SUCCESS;
}

ErrorType Coordinator::readLatest(const Key &key, Value &returnValue, LogEntry &headEntry) {
	DEBUG_COUT(CLASS_NAME, __func__, "Reading key " << key.getId() << " || coordinator " << (int)coordinatorID_);
	HashMaker hashedKey(key.getId());
	size_t bucketID = hashedKey.getHashed();
	Pointer pointer;
	ErrorType eType;

	eType = resolve(bucketID, headEntry);
	if (eType != error::SUCCESS) {
		DEBUG_COUT(CLASS_NAME, __func__, "Resolve failed on bucket " << (int)bucketID << " || coordinator " << (int)coordinatorID_);
		return eType;
	}

	LogEntry entry = headEntry;
	while (true) {
		DEBUG_COUT(CLASS_NAME, __func__, "Searching for key " << key.getId() << " in entry "
				<< entry.getCurrentP().toHexString() << " || coordinator " << (int)coordinatorID_);

		if (entry.getUpdateIfExists(key, returnValue) == true) {
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found as an update in log entry "
					<< entry.getCurrentP().toHexString() << " || coordinator " << (int)coordinatorID_);

			return error::SUCCESS;
		}
		else if (entry.getDependencyIfExists(hashedKey.getHashed(), pointer) == true) {
			DEBUG_COUT (CLASS_NAME, __func__, "Dependency (" << hashedKey.getHashed() << ", "
					<< pointer.toHexString() << ") found in log entry " << entry.getCurrentP().toHexString());
			if ((eType = blockingReadEntry(pointer, entry)) != error::SUCCESS) {
				DEBUG_COUT(CLASS_NAME, __func__, "Can't fetch entry " << pointer.toHexString() << " || coordinator " << (int)coordinatorID_);
				return eType;
			}
		}
		else {
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " not found in bucket "
					<< (int)bucketID << " || coordinator " << (int)coordinatorID_);

			return error::ENTRY_DOES_NOT_EXIST;
			//return eType;
		}
	}
	// impossible to reach here, but required by the compiler
	return error::ENTRY_DOES_NOT_EXIST;
}

ErrorType Coordinator::checkIfSerialized(const LogEntry &entry) {
	// for optimization, we first check the local replica
	ErrorType eType;
	if (entry.getState() == EntryState::SERIALIZED)
		return error::SUCCESS;
	else {
		// now check with other replicas. if any one of them is serialized, the entry is serialized
		DEBUG_COUT(CLASS_NAME, __func__, "Entry " << entry.getCurrentP().toHexString()
				<< " is not locally marked serialized. Must consult other replicas || coordinator " << (int)coordinatorID_);

		// Checking if any of the replicas is serialized (which makes the entry serialized)
		EntryState::State state;
		std::vector<std::promise<ErrorType> > flagErrorPromises(config::MEMORY_SERVER_CNT);
		std::vector<std::future<ErrorType> > flagErrorFutures(config::MEMORY_SERVER_CNT);
		for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
			flagErrorFutures[i] = flagErrorPromises[i].get_future();

			std::shared_ptr<Request> reqPtr (new StateReadRequest(
					flagErrorPromises[i],
					i,
					entry.getCurrentP(),
					state) );

			requestBuffer_->add(reqPtr);

		}
		for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
			eType = flagErrorFutures[i].get();
			if (eType != error::SUCCESS) {
				DEBUG_CERR(CLASS_NAME, __func__, "Error in checking the serialized flag in memory server " << i);
				return eType;
			}
			if (state == EntryState::SERIALIZED) {
				DEBUG_COUT(CLASS_NAME, __func__, "Entry " << entry.getCurrentP().toHexString()
						<< " is marked serialized on replica " << i << " || coordinator " << (int)coordinatorID_);


				// we'll mark the entry  serialized. everybody loves to appear a bit altruistic! (it's not essential)
				eType = replicateState(entry, state);
				if (eType != error::SUCCESS) {
					DEBUG_COUT(CLASS_NAME, __func__, "failed to mark serialized || coordinator " << (int)coordinatorID_);
				}
				return error::SUCCESS;
			}
		}

		// Checking if entry is globally serialized, though it's not marked serialized anywhere.
		size_t n = entry.getDependencies().size();
		std::vector<Pointer> pointer(config::MEMORY_SERVER_CNT * n);
		std::vector<std::promise<ErrorType> > depErrorPromises(config::MEMORY_SERVER_CNT * n);

		std::vector<std::future<ErrorType> > depErrorFutures(config::MEMORY_SERVER_CNT * n);
		for (size_t d = 0; d < entry.getDependencies().size(); d++) {
			size_t bucketID = entry.getDependencies().at(d).getBucketID();
			for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
				depErrorFutures[(i*n) + d] = depErrorPromises[(i*n) + d].get_future();

				std::shared_ptr<Request> reqPtr (new BucketReadRequest(
						depErrorPromises[(i*n) + d],
						i,
						pointer[(i*n) + d],
						(primitive::offset_t)bucketID ) );

				requestBuffer_->add(reqPtr);
			}
		}

		for (size_t d = 0; d < entry.getDependencies().size(); d++) {
			for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
				eType = depErrorFutures[(i*n) + d].get();
				if (eType != error::SUCCESS)
					return eType;
				if (pointer[(i*n) + d].isEqual(entry.getCurrentP()) == false)
					return error::NOT_MARKED_SERIALIZED;
			}
		}

		// again, act a bit altruistic by marking the entry in all replicas serialized
		eType = replicateState(entry, EntryState::SERIALIZED);	// TODO: think about the serialized_failed case
		return error::SUCCESS;
	}
}

ErrorType Coordinator::resolve(const size_t bucketID, LogEntry &headEntry) {
	DEBUG_COUT(CLASS_NAME, __func__, "Resolving bucket " << (int)bucketID << " || coordinator " << (int)coordinatorID_);

	ErrorType eType;
	Pointer head;

	std::map<Pointer, LogEntry>::const_iterator p2eIt;
	std::vector<Dependency>::const_iterator depIt;
	std::set<Pointer>::const_iterator pointerIt;
	std::set<LogEntry>::const_iterator entryIt;

	while (true) {
		std::vector<std::map<size_t, Pointer> > bucketHeads(config::MEMORY_SERVER_CNT);


		/**
		 *
		 * entry = local.log[bucketID]_0
		 *
		 */
		std::promise<ErrorType> eProm;
		std::future<ErrorType> eFut = eProm.get_future();

		DEBUG_COUT(CLASS_NAME, __func__, "Step 1: Reading the local head of bucket " << bucketID);

		std::shared_ptr<Request> reqPtr (new BucketReadRequest(
				eProm,
				localMSIndex_,
				head,
				(primitive::offset_t)bucketID) );

		requestBuffer_->add(reqPtr);

		if ((eType = eFut.get()) != error::SUCCESS)
			return eType;

		if (head.isNull()) {
			headEntry.setToNull();
			return eType;
		}

		if ((eType = blockingReadEntry(head, headEntry)) != error::SUCCESS)
			return eType;

		/**
		 *
		 * If (entry.status == SERIALIZED)	return e
		 * If (entry.status == FAILED)		return e.dep[b]
		 *
		 */
		DEBUG_COUT(CLASS_NAME, __func__, "Step 2: Checking the serialized flag of entry " << headEntry.getCurrentP().toHexString());
		if (headEntry.getState() == EntryState::SERIALIZED) {
			DEBUG_COUT(CLASS_NAME, __func__, "Entry " << headEntry.getCurrentP().toHexString() << " is serialized.");
			return error::SUCCESS;
		}
		if (headEntry.getState() == EntryState::FAILED) {
			DEBUG_COUT(CLASS_NAME, __func__, "Entry " << headEntry.getCurrentP().toHexString() << " has failed.");
			if (headEntry.getDependencyIfExists(bucketID, head) == true) {
				if ((eType = blockingReadEntry(head, headEntry)) != error::SUCCESS)
						return eType;
				return error::SUCCESS;
			}
			else return error::KEY_NOT_FOUND;

		}


		/**
		 *
		 * Collect D* where
		 * D_0 = U_{r in R} r.log[i]_0
		 * do {
		 *    D_i = U_{r in R, b in n.bdeps, n in D_[i-1]} r.log[b]_0
		 * } while(D_i != D_i-1);
		 *
		 */

		/**
		 * First:
		 * D_0 = U_{r in R} r.log[i]_0
		 *
		 */
		std::set<size_t> bdepsUnion;
		std::set<Pointer> D_pointers;
		std::map<Pointer, LogEntry> D_map;

		DEBUG_COUT(CLASS_NAME, __func__, "Step 3: Getting all the entries who are resident in bucket " << bucketID);

		bdepsUnion.insert(bucketID);
		eType = readBucketHeadsFromAllReplicas(bdepsUnion, bucketHeads);
		if (eType != error::SUCCESS) {
			DEBUG_COUT(CLASS_NAME, __func__, "\t Can't read bucket heads of all replicas");
			return eType;
		}
		for (size_t i = 0; i < bucketHeads.size(); i++)
			D_pointers.insert(bucketHeads[i][bucketID]);


		// getting entries from pointers
		for (pointerIt = D_pointers.begin(); pointerIt != D_pointers.end(); pointerIt++) {
			DEBUG_COUT(CLASS_NAME, __func__, "Inserting entry " << pointerIt->toHexString() << " in D_0");
			LogEntry entry;
			eType = blockingReadEntry(*pointerIt, entry);
			if (eType != error::SUCCESS) {
				DEBUG_COUT(CLASS_NAME, __func__, "\t Can't read log entry " << pointerIt->toHexString());
				return eType;
			}
			D_map[*pointerIt] = entry;
		}


		/**
		 * Then:
		 * do {
		 *    D_i = U_{r in R, b in n.bdeps, n in D_[i-1]} r.log[b]_0
		 * } while(D_i != D_i-1);
		 *
		 */
		DEBUG_COUT(CLASS_NAME, __func__, "Step 4: Expanding D to find its closure");
		std::vector<size_t> additions;
		int iteration = 0;
		do {
			DEBUG_COUT(CLASS_NAME, __func__, "Iteration " << (iteration + 1));

			additions.clear();

			for (p2eIt = D_map.begin(); p2eIt != D_map.end(); p2eIt++) {
				const LogEntry &entry = p2eIt->second;

				// TODO: Think whether this is correct to ignore already serialized entries (line below)
				if (checkIfSerialized(entry) == error::SUCCESS)
					continue;

				for (depIt = entry.getDependencies().begin(); depIt != entry.getDependencies().end(); depIt++) {
					if (bdepsUnion.find(depIt->getBucketID()) == bdepsUnion.end()) {
						DEBUG_COUT(CLASS_NAME, __func__, "Add bucketID " << depIt->getBucketID() << " from entry " << entry.getCurrentP().toHexString());
						// the pointer is not already in the set
						additions.push_back(depIt->getBucketID());
						bdepsUnion.insert(depIt->getBucketID());
					}
				}
			}

			if (! additions.empty()) {
				// read the bucket heads for the buckets in additions
				eType = readBucketHeadsFromAllReplicas<std::vector<size_t> >(additions, bucketHeads);
				if (eType != error::SUCCESS) {
					DEBUG_COUT(CLASS_NAME, __func__, "Can't read bucket heads of all replicas");
					return eType;
				}

				// putting all the bucket heads into a set to remove duplicates.
				std::set<Pointer> pointersSet;
				for (std::vector<std::map<size_t, Pointer> >::const_iterator vecIt = bucketHeads.begin(); vecIt != bucketHeads.end(); vecIt++) {
					for (std::map<size_t, Pointer>::const_iterator mapIt = vecIt->begin(); mapIt != vecIt->end(); mapIt++)
						pointersSet.insert(mapIt->second);
				}

				// add the entries
				for (pointerIt = pointersSet.begin(); pointerIt != pointersSet.end(); pointerIt++) {
					if (D_map.find(*pointerIt) == D_map.end()) {
						DEBUG_COUT(CLASS_NAME, __func__, "Pointer " << pointerIt->toHexString() << " added to D_" << iteration);
						LogEntry entry;
						eType = blockingReadEntry(*pointerIt, entry);
						if (eType != error::SUCCESS) {
							DEBUG_COUT(CLASS_NAME, __func__, "Can't read log entry " << pointerIt->toHexString());
							return eType;
						}
						D_map[*pointerIt] = entry;
					}
				}
			}
			iteration++;
		} while(!additions.empty());


		/**
		 *
		 * For each non-failed e in D*: try_serialize(e)
		 * Mark rest of D* as FAILED
		 *
		 */
		// TODO currently, actions on entries are done sequentially. Ideal would be to do them in parallel
		for (p2eIt = D_map.begin(); p2eIt != D_map.end(); p2eIt++) {
			const LogEntry &e = p2eIt->second;
			if (isFailed (e, bucketHeads, D_map)) {
				eType = replicateState(e, EntryState::FAILED);
				if (eType == error::SUCCESS)
					DEBUG_COUT(CLASS_NAME, __func__, "Entry " << e.getCurrentP().toHexString() << " is marked FAILED on all replicas");
				else
					DEBUG_COUT(CLASS_NAME, __func__, "Error in marking entry " << e.getCurrentP().toHexString() << " FAILED on all replicas");
			}
			else {
				eType = finishMakingSerialized(e, bucketHeads, D_map);
				if (eType == error::SUCCESS)
					DEBUG_COUT(CLASS_NAME, __func__, "Entry " << e.getCurrentP().toHexString() << " is marked SERIALIZED on all replicas");
				else
					// TODO: since we rely on the fact that a log entry must be marked serialized everywhere to be able be depended on,
					// we must make sure that it's marked serialized everywhere.
					DEBUG_COUT(CLASS_NAME, __func__, "Error in marking entry " << e.getCurrentP().toHexString() << " SERIALIZED on all replicas");
			}
		}
	}
	return error::RESOLVE_FAILED;
}

template <typename container>
ErrorType Coordinator::readBucketHeadsFromAllReplicas(const container &readBuckets, std::vector<std::map<size_t, Pointer> > &bucketHeads) {
	ErrorType eType;
	size_t bucketID;

	size_t n = readBuckets.size();
	std::vector<std::promise<ErrorType> >	errorPromises(config::MEMORY_SERVER_CNT * n);
	std::vector<std::future<ErrorType> >	errorFutures (config::MEMORY_SERVER_CNT * n);

	for (size_t r = 0; r < config::MEMORY_SERVER_CNT; r++) {
		size_t i = 0;
		for (typename container::const_iterator it = readBuckets.begin(); it != readBuckets.end(); it++) {
			errorFutures[(r * n) + i] = errorPromises[(r * n) + i].get_future();
			bucketID = *it;

			std::shared_ptr<Request> reqPtr (new BucketReadRequest(
					errorPromises[(r * n) + i],
					r,
					bucketHeads[r][bucketID],
					(primitive::offset_t)bucketID) );

			requestBuffer_->add(reqPtr);
			++i;
		}
	}

	for (size_t r = 0; r < config::MEMORY_SERVER_CNT; r++) {
		size_t i = 0;
		for (typename container::const_iterator it = readBuckets.begin(); it != readBuckets.end(); it++){
			eType = errorFutures[(r * n) + i].get();
			if (eType != error::SUCCESS) {
				bucketID = *it;
				DEBUG_CERR(CLASS_NAME, __func__, "Failed to read bucket head " << bucketID << " from replica " << r);
				return eType;
			}
			++i;
		}
	}
	return error::SUCCESS;
}

ErrorType Coordinator::blockingReadEntry(const Pointer &pointer, LogEntry &entry) {
	DEBUG_COUT (CLASS_NAME, __func__, "Requesting the log entry for " << pointer.toHexString());

	if (pointer.isNull()) {
		DEBUG_COUT(CLASS_NAME, __func__, "Log Entry " << pointer.toHexString() << " is NULL.");
		return error::ENTRY_DOES_NOT_EXIST;
	}


	std::promise<ErrorType> errorPromise;
	std::future<ErrorType> errorFuture;
	errorFuture = errorPromise.get_future();

	std::shared_ptr<Request> reqPtr (new EntryReadRequest(
				errorPromise,
				localMSIndex_,
				pointer.getCoordinatorNum(),
				entry,
				pointer.getOffset(),
				pointer.getLength() ) );

	requestBuffer_->add(reqPtr);

	ErrorType eType = errorFuture.get();
	if (eType != error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "Can't read " << pointer.toHexString());

	return eType;
}

bool Coordinator::isFailed (const LogEntry &e, const std::vector<std::map<size_t, Pointer> > &collectedBucketHeads, const std::map<Pointer, LogEntry> &pointerToEntryMap) const{
	// first check the flag
	if (e.getState() == EntryState::FAILED)
		return true;

	// e has failed: EXISTS r in R, b in e.bdeps WHERE r.log[b] > e AND e not in r.log[b]
	std::vector<Dependency>::const_iterator depIt;
	std::vector<std::map<size_t, Pointer> >::const_iterator repIt;
	for (depIt = e.getDependencies().begin(); depIt != e.getDependencies().end(); depIt++) {
		for (repIt = collectedBucketHeads.begin(); repIt != collectedBucketHeads.end(); repIt++) {
			const Pointer &pHead = repIt->at(depIt->getBucketID());
			const LogEntry &eHead = pointerToEntryMap.at(pHead);
			if (e < eHead) {
				// TODO: here, we assume that dependencies are always marked serialized.
				// If we don't have this assumption, then we need to follow down to see if e in r.log[b] or not
				if (e.getState() != EntryState::SERIALIZED)
					return true;
			}
		}
	}
	return false;
}

ErrorType Coordinator::finishMakingSerialized(const LogEntry &e, const std::vector<std::map<size_t, Pointer> > &collectedBucketHeads, const std::map<Pointer, LogEntry> &pointerToEntryMap) {
	ErrorType eType;
	const size_t n = e.getDependencies().size();
	std::vector<std::promise<ErrorType> > errorPromises(n * config::MEMORY_SERVER_CNT);
	std::vector<std::future<ErrorType> > errorFutures(n * config::MEMORY_SERVER_CNT);
	std::vector<Pointer> actualHeads(config::MEMORY_SERVER_CNT * n);

	for (size_t r = 0; r < config::MEMORY_SERVER_CNT; r++) {
		for (size_t d = 0; d < n; d++){
			errorFutures[(r * n) + d] = errorPromises[(r * n) + d].get_future();

			const Dependency &dep = e.getDependencies().at(d);
			const size_t bID =  dep.getBucketID();
			const Pointer &oldHead = collectedBucketHeads[r].at(bID);
			const LogEntry &entryHead = pointerToEntryMap.at(oldHead);

			if (oldHead.isEqual(e.getCurrentP()))
				// the head is already owned by the entry, no need to CAS
				errorPromises[(r * n) + d].set_value(error::SUCCESS);

			else if (oldHead.isEqual(dep.getPointer() ) ) {
				// Valid CAS: Prepend
				std::shared_ptr<Request> reqPtr (new CASRequest(
						errorPromises[(r * n) + d],
						r,
						oldHead,
						e.getCurrentP(),
						(primitive::offset_t)bID,
						actualHeads[(r * n) + d] ) );

				requestBuffer_->add(reqPtr);
			}

			else if (e > entryHead && isFailed(entryHead, collectedBucketHeads, pointerToEntryMap) ) {
				// Valid CAS: Replace
				std::shared_ptr<Request> reqPtr (new CASRequest(
						errorPromises[(r * n) + d],
						r,
						oldHead,
						e.getCurrentP(),
						(primitive::offset_t)bID,
						actualHeads[(r * n) + d] ) );

				requestBuffer_->add(reqPtr);
			}
			else
				// invalid CAS
				return error::UNSERIALIZABLE;
		}
	}

	for (size_t r = 0; r < config::MEMORY_SERVER_CNT; r++) {
		for (size_t d = 0; d < n; d++){
			eType = errorFutures[(r * n) + d].get();
			if (eType != error::SUCCESS) {
				DEBUG_CERR(CLASS_NAME, __func__, "CAS failed for dependency ("
						<< e.getDependencies().at(d).toString() << ") on memory server "<< r);
				return eType;
			}
			else if (actualHeads[(r * n) + d].isEqual(e.getCurrentP()))
				// Since we didn't do any CAS here
				continue;
			else
				DEBUG_COUT(CLASS_NAME, __func__, "CAS " << n << " hash bucket(s) on memory server "
						<< r << " by coordinator " << (int)coordinatorID_);
		}
	}
	DEBUG_COUT(CLASS_NAME, __func__, "Published the change by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}


//ErrorType Coordinator::readByKey(const Key key, const SCN scn, const TID tid, Value &returnValue) {
//	return error::SUCCESS;
//	(void)scn;	// TODO: must be removed
//	(void)tid;	// TODO: must be removed
//
//	HashMaker hashedKey(key.getId());
//	LogEntry entry;
//	Pointer p;
//	Value v;
//
//	ErrorType eType = memoryServerCtxs_.at(localMSCtxIndex_).readBucketHash(hashedKey.getHashed(), p);
//	DEBUG_COUT(CLASS_NAME, __func__, "Reading bucket hash for key " << key.getId() << " resulted pointer: "
//			<< p.toHexString() << " by coordinator " << (int)coordinatorID_);
//
//	if (eType == error::KEY_NOT_FOUND)
//		return eType;
//
//	searchDepth = 1;
//
//	while (true) {
//		error::Throwable::testError(memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(p, entry));
//		if (entry.getUpdateIfExists(key, v) == true) {
//			// TODO: check if the KV pair fulfills the SCN and TID conditions
//			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found as an update in log entry " << p.toHexString()
//					<< " by coordinator " << (int)coordinatorID_);
//			break;
//		}
//		else if (entry.getDependencyIfExists(hashedKey.getHashed(), p) == true) {
//			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found as a dependency in log entry "
//					<< p.toHexString() << " by coordinator " << (int)coordinatorID_);
//
//			searchDepth++;
//			continue;
//		}
//		else {
//			// no cleanup is needed
//			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found neither as a dependency nor an update in log entry "
//					<< p.toHexString() << " by coordinator " << (int)coordinatorID_);
//			return error::KEY_NOT_FOUND;
//		}
//	}
//	returnValue = v;
//	return error::SUCCESS;
//}

std::string Coordinator::printChange(const std::vector<Dependency> &deps, const std::vector<KeyValue> &updates) {
	std::string str = "";
	str += "Updates: \"";
	for (size_t i = 0; i < updates.size(); i++){
		str += updates.at(i).toString();
		str += ", ";
	}

	str += "\", Dependencies: \"";

	for (size_t i = 0; i < deps.size(); i++){
		str += utilities::ToString<size_t>(deps.at(i).getBucketID()) + "(" + deps.at(i).getPointer().toHexString() + ")";
		str += ", ";
	}
	str += "\"";
	return str;
}
