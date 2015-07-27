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
#include <future>         // std::promise, std::future


#define CLASS_NAME	"Coord"


Coordinator::Coordinator(primitive::coordinator_num_t coordNum) : coordinatorID_(coordNum) {
	// since coordinatorID_ is const, it must be initialized using constructor initialization list
	generationNum_ = 0;
	freeBufferOffset_ = (uint32_t)0;
	localMSCtxIndex_ = coordinatorID_;
	DEBUG_COUT(CLASS_NAME, __func__, "Coordinator created with ID " << (int)coordinatorID_);
}

Coordinator::~Coordinator() {
	DEBUG_COUT(CLASS_NAME, __func__, "Coordinator destroyed with ID " << (int)coordinatorID_);
}

ErrorType Coordinator::connectToMemoryServers(std::vector<MemoryServerContext> memoryServerCtxs){
	this->memoryServerCtxs_ = memoryServerCtxs;
	DEBUG_COUT(CLASS_NAME, __func__, "Memory server contexts are all set for coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::applyChange(Change &change, TID tID, Pointer &newEntryPointer){
	DEBUG_COUT(CLASS_NAME, __func__, "Applying Change: " << change.toString() << " by coordinator " << (int)coordinatorID_);

	if (change.getUpdates().size() == 0) {
		DEBUG_COUT(CLASS_NAME, __func__, "Change does not contain any update");
		return error::NO_UPDATE_KEY_IN_CHANGE;
	}

	(void)tID;	// TODO: since it's unused

	LogEntry *entry = NULL;
	Pointer *entryPointer = NULL;

	error::Throwable::testError(createNewPointer(change, &entryPointer));
	error::Throwable::testError(makeNewLogEntry(change, *entryPointer, &entry));
	error::Throwable::testError(propagateLogEntry(*entry));
	error::Throwable::testError(publishChanges(*entry));
	error::Throwable::testError(makeSerialized(*entry));

	newEntryPointer = *entryPointer;	// for logging purposes. Otherwise, the variable 'newEntryPointer' can be removed (also from the function signature)

	DEBUG_COUT(CLASS_NAME, __func__, "Change successfully applied by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::createNewPointer(Change &change, Pointer **pointer) {

	primitive::entry_size_t entrySize = LogEntry::calculateEntrySize(change.getDependencies(), change.getUpdates());
	*pointer = new Pointer(coordinatorID_, generationNum_, entrySize, freeBufferOffset_);

	freeBufferOffset_ += entrySize;	// moving forward the free buffer head

	DEBUG_COUT(CLASS_NAME, __func__, "Pointer created (" << (*pointer)->toHexString()
			<< ") by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::makeNewLogEntry(Change &change, Pointer &entryPointer, LogEntry **entry __attribute__((unused))) const{
	bool isSerialized = false;

	*entry = new LogEntry(change.getDependencies(), change.getUpdates(), entryPointer, isSerialized);
	DEBUG_COUT(CLASS_NAME, __func__, "Log entry created with pointer = "
			<< (*entry)->getCurrentP().toHexString() << " by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::propagateLogEntry(LogEntry &entry){
	std::vector<std::promise<ErrorType> > errorPromises(memoryServerCtxs_.size());
	std::vector<std::future<ErrorType> > errorFutures(memoryServerCtxs_.size());
	ErrorType eType;

	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		errorFutures[i] = errorPromises[i].get_future();
		memoryServerCtxs_.at(i).writeLogEntry(coordinatorID_, entry, errorPromises[i]);
	}

	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		eType = errorFutures[i].get();
		if (eType != error::SUCCESS)
			return eType;
	}

	DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString()
			<< " propagated to all replicas by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}
/*
ErrorType Coordinator::publishChanges(LogEntry &entry){
	std::vector<Dependency> dependencies = entry.getDependencies();

	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		std::vector<size_t> bucketIDs;
		std::vector<Pointer> expectedHeads;
		std::vector<Pointer> actualCurrentHeads;

		for (size_t d = 0; d < dependencies.size(); d++){
			ErrorType eType = memoryServerCtxs_.at(i).swapMultipleBucketHash(
					bucketIDs,
					expectedHeads,
					entry.getCurrentP(),
					actualCurrentHeads);

			if (eType != error::SUCCESS) {
				DEBUG_CERR(CLASS_NAME, __func__, "CAS failed for dependency ("
						<< dependencies.at(d).toString() << ") on memory server context " << i );
				return eType;
			}

		}
		DEBUG_COUT(CLASS_NAME, __func__, "CAS " << dependencies.size() << " hash bucket(s) in memory server "
				<< i << " by coordinator " << (int)coordinatorID_);
	}

	DEBUG_COUT(CLASS_NAME, __func__, "Published the change by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}
*/
ErrorType Coordinator::publishChanges(LogEntry &entry){
	ErrorType eType;
	//primitive::pointer_size_t actualHead;
	std::vector<Dependency> dependencies = entry.getDependencies();
	size_t n = dependencies.size();

	std::vector<std::promise<ErrorType> >	errorPromises(memoryServerCtxs_.size() * n);
	std::vector<std::future<ErrorType> >	errorFutures (memoryServerCtxs_.size() * n);

//	std::vector<std::promise<primitive::pointer_size_t> > actualHeadPromises(memoryServerCtxs_.size() * n);
//	std::vector<std::future<primitive::pointer_size_t> > 	actualHeadFutures (memoryServerCtxs_.size() * n);

	std::vector<Pointer> actualHeads(memoryServerCtxs_.size() * n);

	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		for (size_t d = 0; d < dependencies.size(); d++){
			errorFutures[(i * n) + d] = errorPromises[(i * n) + d].get_future();
			// actualHeadFutures[(i * n) + d] = actualHeadPromises[(i * n) + d].get_future();

			memoryServerCtxs_.at(i).swapBucketHash(
					dependencies.at(d).getBucketID(),
					dependencies.at(d).getPointer(),
					entry.getCurrentP(),
					errorPromises[(i * n) + d],
					//actualHeadPromises[(i * n) + d]);
					actualHeads[(i * n) + d]);
		}
	}

	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		for (size_t d = 0; d < dependencies.size(); d++){
			eType		= errorFutures[(i * n) + d].get();
			//actualHead	= actualHeadFutures[(i * n) + d].get();
			if (eType != error::SUCCESS) {
				DEBUG_CERR(CLASS_NAME, __func__, "CAS failed for dependency ("
						<< dependencies.at(d).toString() << ") on memory server context "<< i);
				return eType;
			}
			DEBUG_COUT(CLASS_NAME, __func__, "CAS " << dependencies.size() << " hash bucket(s) in memory server "
					<< i << " by coordinator " << (int)coordinatorID_);
		}
	}
	DEBUG_COUT(CLASS_NAME, __func__, "Published the change by coordinator " << (int)coordinatorID_);
	return error::SUCCESS;
}

ErrorType Coordinator::makeSerialized(LogEntry &entry) {
	std::vector<std::promise<ErrorType> >	errorPromises(memoryServerCtxs_.size());
	std::vector<std::future<ErrorType> >	errorFutures (memoryServerCtxs_.size());
	ErrorType eType;

	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		errorFutures[i ] = errorPromises[i].get_future();
		memoryServerCtxs_.at(i).markSerialized(entry, errorPromises[i]);
	}

	for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
		eType	= errorFutures[i].get();
		if (eType != error::SUCCESS)
			// TODO what should we do here?
			return eType;
		else
			DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString()
					<< " marked serialized on log journal[" << (int)coordinatorID_ << "][" << i << "]");
	}

	DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString()
			<< " serialized on all replicas by coordinator " << (int)coordinatorID_);

	return error::SUCCESS;
}

ErrorType Coordinator::readLatest(const Key &key, Value &returnValue, Pointer &bucketHead) {
	HashMaker hashedKey(key.getId());
	Pointer p;
	LogEntry entry;
	ErrorType eType;
	bool firstEntryInChain = true;	// we make use of the fact that once an entry is depended on, it must be serialzied

	std::promise<ErrorType> eProm;
	std::future<ErrorType> eFut = eProm.get_future();

	memoryServerCtxs_.at(localMSCtxIndex_).readBucketHash(hashedKey.getHashed(), eProm, bucketHead);
	eType = eFut.get();
	if (eType != error::SUCCESS)
		return eType;

	DEBUG_COUT(CLASS_NAME, __func__, "Reading bucket hash for key " << key.getId() << " resulted pointer: "
			<< bucketHead.toHexString() << " || coordinator " << (int)coordinatorID_);

	p = bucketHead;

	while (true) {
		std::promise<ErrorType> errorProm;
		std::future<ErrorType> errorFut = errorProm.get_future();
		memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(p, errorProm, entry);
		eType = errorFut.get();
		if (eType != error::SUCCESS)
			return eType;

		if (entry.getUpdateIfExists(key, returnValue) == true) {
			// TODO: check if the KV pair fulfills the SCN and TID conditions
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found as an update in log entry " << p.toHexString()
					<< " || coordinator " << (int)coordinatorID_);

			// in two cases, a log entry is considered serialized:
			// - the entry is already depended on by another entry (i.e. it's not the head of the bucket). It may or may not have been marked serialized
			// - the entry is at the head of the list and is serialized
			if (firstEntryInChain == false || checkIfSerialized(entry) == error::SUCCESS) {
				DEBUG_COUT(CLASS_NAME, __func__, "Entry " << entry.getCurrentP().toHexString()
						<< " is marked serialized. The latest value for key "
						<< key.getId() << " is " << returnValue.getContent() << " || coordinator " << (int)coordinatorID_);
				return error::SUCCESS;
			}
			else {
				DEBUG_COUT(CLASS_NAME, __func__, "The entry " << entry.getCurrentP().toHexString()
						<< " is not marked serialized. Must be resolved || coordinator " << (int)coordinatorID_);

				eType = resolveConflict(hashedKey.getHashed(), p);

				std::promise<ErrorType> errorProm2;
				std::future<ErrorType> errorFut2 = errorProm2.get_future();
				if (eType == error::SUCCESS) {
					memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(p, errorProm2, entry);
					eType = errorFut2.get();
					if (eType != error::SUCCESS)
						return eType;

					entry.getUpdateIfExists(key, returnValue);
					return error::SUCCESS;
				}
				else
					memoryServerCtxs_.at(localMSCtxIndex_).readBucketHash(hashedKey.getHashed(), errorProm2, p);
					eType = errorFut2.get();
					if (eType != error::SUCCESS)
						return eType;
					continue;
			}
		}
		else if (entry.getDependencyIfExists(hashedKey.getHashed(), p) == true) {
			// note that we store the new pointer in p, so the next iteration of the loop will probe the entry by which the current one depends on.
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found as a dependency in entry "
					<< p.toHexString() << " || coordinator " << (int)coordinatorID_);

			firstEntryInChain = false;
			continue;
		}
		else {
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found neither as a dependency nor an update in log entry "
					<< p.toHexString() << " || coordinator " << (int)coordinatorID_);
			return error::KEY_NOT_FOUND;
		}
	}
}

ErrorType Coordinator::checkIfSerialized(LogEntry &entry) {
	// for optimization, we first check the local replica
	ErrorType eType;
	if (entry.isSerialized())
		return error::SUCCESS;
	else {
		// now check with other replicas. if any one of them is serialized, the entry is serialized
		DEBUG_COUT(CLASS_NAME, __func__, "Entry " << entry.getCurrentP().toHexString()
				<< " is not locally marked serialized. has to consult other replicas || coordinator " << (int)coordinatorID_);


		// Checking if any of the replicas is serialized (which makes the entry is serialized)
		bool serializedFlag;
		std::vector<std::promise<ErrorType> > flagErrorPromises(memoryServerCtxs_.size());
		std::vector<std::future<ErrorType> > flagErrorFutures(memoryServerCtxs_.size());
		for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
			flagErrorFutures[i] = flagErrorPromises[i].get_future();
			memoryServerCtxs_.at(i).checkSerialized(entry, flagErrorPromises[i], serializedFlag);
		}
		for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
			eType = flagErrorFutures[i].get();
			if (eType != error::SUCCESS) {
				DEBUG_CERR(CLASS_NAME, __func__, "Error in checking the serialized flag in memory server " << i);
				return eType;
			}
			if (serializedFlag == true) {
				DEBUG_COUT(CLASS_NAME, __func__, "Entry " << entry.getCurrentP().toHexString()
						<< " is marked serialized on replica " << i << " || coordinator " << (int)coordinatorID_);


				// we'll mark the entry  serialized. everybody loves to appear a bit altruistic! (it's not essential)
				eType = makeSerialized(entry);
				if (eType != error::SUCCESS) {
					DEBUG_COUT(CLASS_NAME, __func__, "failed to mark serialized || coordinator " << (int)coordinatorID_);
				}
				return error::SUCCESS;
			}
		}

		// Checking if entry is globally serialized, though it's not marked serialized anywhere.
		Pointer pointer;
		size_t n = entry.getDependencies().size();
		std::vector<std::promise<ErrorType> > depErrorPromises(memoryServerCtxs_.size() * n);
		std::vector<std::future<ErrorType> > depErrorFutures(memoryServerCtxs_.size() * n);
		for (size_t d = 0; d < entry.getDependencies().size(); d++) {
			size_t bucketID = entry.getDependencies().at(d).getBucketID();
			for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
				depErrorFutures[(i*n) + d] = depErrorPromises[(i*n) + d].get_future();
				memoryServerCtxs_.at(i).readBucketHash(bucketID, depErrorPromises[(i*n) + d], pointer);
			}
		}

		for (size_t d = 0; d < entry.getDependencies().size(); d++) {
			size_t bucketID = entry.getDependencies().at(d).getBucketID();
			for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
				eType = depErrorFutures[(i*n) + d].get();
				if (eType != error::SUCCESS)
					return eType;
				if (pointer.isEqual(entry.getCurrentP()) == false)
					return error::NOT_MARKED_SERIALIZED;
			}
		}

		// again, act a bit altruistic by marking the entry in all replicas serialized
		eType = makeSerialized(entry);
		return error::SUCCESS;
	}
}

ErrorType Coordinator::resolveConflict(const size_t bucketID, Pointer &newHead) {
	return error::SUCCESS;
	/*
	DEBUG_COUT(CLASS_NAME, __func__, "Resolving conflict for bucket " << (int)bucketID << " || coordinator " << (int)coordinatorID_);

	Pointer bucketHead;
	ErrorType eType;
	LogEntry entry;

	eType = memoryServerCtxs_.at(localMSCtxIndex_).readBucketHash(bucketID, bucketHead);
	entry = memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(bucketHead, entry);

	size_t numOfDependencies = entry.getDependencies().size();
	std::vector<std::vector<Pointer> > currentHeads(memoryServerCtxs_.size(), std::vector<Pointer>(numOfDependencies));

	// now check if globally serialized, though it's not marked serialized anywhere.
	for (size_t d = 0; d < entry.getDependencies().size(); d++) {
		size_t bucketID = entry.getDependencies().at(d).getBucketID();
		for (size_t i = 0; i < memoryServerCtxs_.size(); i++) {
			eType = memoryServerCtxs_.at(i).readBucketHash(bucketID, currentHeads[i][d]);
			if (eType != error::SUCCESS)
				return eType;
		}
	}

	DEBUG_COUT(CLASS_NAME, __func__, "Read all the hash buckets of dependencies || coordinator " << (int)coordinatorID_);

	std::set<LogEntry> eligibleCandidates;
	getEligibleCandidates(currentHeads, eligibleCandidates);

	// Printing the eligible candidates for logging puposes
	std::string concat = "";
	for (std::set<Pointer>::iterator it = eligibleCandidates.begin(); it != eligibleCandidates.end(); it++) {
		concat += it->toHexString() + " , ";
	}
	DEBUG_COUT(CLASS_NAME, __func__, "There are " << eligibleCandidates.size()
			<< " eligible candidates: " << concat << " || coordinator " << (int)coordinatorID_);

	//
	size_t bucketIndex = -1;
	if (eligibleCandidates.size() == 0) {
		// no one is eligible to be the head of the bucket. Therefore, it must be reverted back to the latest serialized value
		Pointer toLastSerializedEntry;
		entry.getDependencyIfExists(bucketID, toLastSerializedEntry);
		// we need the index of the intended bucketID in the list of dependencies, as we would like to use it for CAS.
		for (size_t d = 0; d < entry.getDependencies().size(); d++) {
			if (entry.getDependencies().at(d).getBucketID() == bucketID) {
				bucketIndex = d;
				break;
			}
		}

		// Reverting the value of the bucketID in all replicas to its latest serialized entry
		for (size_t i = 0; i < memoryServerCtxs_.size(); i++){
			Pointer actualHead;
			eType = memoryServerCtxs_.at(i).swapBucketHash(bucketID, currentHeads[i][bucketIndex], toLastSerializedEntry, actualHead);
			if (eType != error::SUCCESS && actualHead.isEqual(toLastSerializedEntry) == false) {
				std::cerr << "Failed when reverting back to the old serialized value" << std::endl;
				resolveConflict(bucketID, newHead);
			}
		}
		newHead = toLastSerializedEntry;
		DEBUG_COUT(CLASS_NAME, __func__, "Successfully reverted bucket " << (int)bucketID
				<< " to the serialized value " << newHead <<  " || coordinator " << (int)coordinatorID_);
	}
	else if (eligibleCandidates.size() == 1) {
		DEBUG_COUT(CLASS_NAME, __func__, "Entry " << entry.getCurrentP().toHexString()
				<< " is serialized, although it's not marked serialized on any replica || coordinator " << (int)coordinatorID_);

		eType = makeSerialized(entry);
		if (eType != error::SUCCESS) {
			DEBUG_COUT(CLASS_NAME, __func__, "failed to mark serialized || coordinator " << (int)coordinatorID_);
		}
		newHead = *eligibleCandidates.begin();
		return error::SUCCESS;
	}
	else {
		LogEntry winnerEntry;
		Pointer pointer;
		chooseWinnerEntry(eligibleCandidates, winnerEntry);
		if (winnerEntry.getDependencyIfExists(bucketID, pointer) == false) {
			// The winner entry doesn't have bucketID as dependency.
			// Therefore, we revert back the bucket to point to the last serialized entry


		}
		eType = memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(newHead, winnerEntry);
		if (eType != error::SUCCESS)
			return eType;

		for (size_t d = 0; d < winnerEntry.getDependencies().size(); d++) {
			for (size_t i = 0; i < memoryServerCtxs_.size(); i++){
				Pointer actualHead;
				eType = memoryServerCtxs_.at(i).swapBucketHash(bucketID, currentHeads[i][bucketIndex], toLastSerializedEntry, actualHead);
				if (eType != error::SUCCESS && actualHead.isEqual(toLastSerializedEntry) == false) {
					std::cerr << "Failed when reverting back to the old serialized value" << std::endl;
					resolveConflict(bucketID, newHead);
				}
			}
		}

		newHead = toLastSerializedEntry;
		DEBUG_COUT(CLASS_NAME, __func__, "Successfully reverted bucket " << (int)bucketID
				<< " to the serialized value " << newHead <<  " || coordinator " << (int)coordinatorID_);
	}
	*/
}


void Coordinator::getEligibleCandidates(const std::vector<std::vector<Pointer> > &currentHeads, std::set<LogEntry> &candidates) const{
	/*
	std::set<Pointer> eligibleCandidates;
	LogEntry entry;

	// first, initialize the eligible list with all the pointers of all the buckets.
	// We use set as we don't want to process the same log entry twice,
	// i.e. once a log entry is (is not) eligible, it remains eligible (non-eligible)
	for (size_t i = 0; i < currentHeads.size(); i++)
		for (size_t j = 0; j < currentHeads.at(i).size(); j++)
			eligibleCandidates.insert(currentHeads[i][j]);


	if (eligibleCandidates.size() == 1) {
		ErrorType e = memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(*eligibleCandidates.begin(), entry);
		candidates.insert(entry);
		return;
	}

	// According to definition, a log entry is eligible only and only if all its dependencies
	// point to the latest serialized entries of their corresponding buckets.
	for (std::set<Pointer>::iterator it = eligibleCandidates.begin(); it != eligibleCandidates.end(); it++) {
		ErrorType e = memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(*it, entry);
		if (checkIfEligible(entry))
			candidates.insert(entry);
	}
	*/
}

bool Coordinator::checkIfEligible(const LogEntry &entry) const {
	return true;
	/*
	// check if all the entry's dependencies point to the latest serialized entries of their corresponding buckets.
	size_t bucketID;
	Pointer head;
	ErrorType eType;

	const std::vector<Dependency>& deps = entry.getDependencies();
	for (size_t d = 0; d < deps.size(); d++){
		bucketID = deps.at(d).getBucketID();
		eType = memoryServerCtxs_.at(localMSCtxIndex_).readBucketHash(bucketID, head);
		if (head.isEqual(deps.at(d).getPointer())
				|| head.isEqual(entry.getCurrentP()))
			continue;
		else {
			// bucket
			return false;
		}
	}
	return true;
	*/
}

void Coordinator::chooseWinnerEntry(const std::set<LogEntry> &candidates, LogEntry &winner) const {
	std::set<LogEntry>::iterator	it;
	std::set<LogEntry>::iterator	max;
	primitive::entry_size_t			maxSize = 0;

	for (it = candidates.begin(); it != candidates.end(); it++) {
		if (it->getCurrentP().getLength() > maxSize)
			max = it;
	}
	winner = *max;
}

ErrorType Coordinator::readByKey(const Key key, const SCN scn, const TID tid, Value &returnValue, int &searchDepth) {
	return error::SUCCESS;
	/*
	(void)scn;	// TODO: must be removed
	(void)tid;	// TODO: must be removed

	HashMaker hashedKey(key.getId());
	LogEntry entry;
	Pointer p;
	Value v;

	ErrorType eType = memoryServerCtxs_.at(localMSCtxIndex_).readBucketHash(hashedKey.getHashed(), p);
	DEBUG_COUT(CLASS_NAME, __func__, "Reading bucket hash for key " << key.getId() << " resulted pointer: "
			<< p.toHexString() << " by coordinator " << (int)coordinatorID_);

	if (eType == error::KEY_NOT_FOUND)
		return eType;

	searchDepth = 1;

	while (true) {
		error::Throwable::testError(memoryServerCtxs_.at(localMSCtxIndex_).readLogEntry(p, entry));
		if (entry.getUpdateIfExists(key, v) == true) {
			// TODO: check if the KV pair fulfills the SCN and TID conditions
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found as an update in log entry " << p.toHexString()
					<< " by coordinator " << (int)coordinatorID_);
			break;
		}
		else if (entry.getDependencyIfExists(hashedKey.getHashed(), p) == true) {
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found as a dependency in log entry "
					<< p.toHexString() << " by coordinator " << (int)coordinatorID_);

			searchDepth++;
			continue;
		}
		else {
			// no cleanup is needed
			DEBUG_COUT(CLASS_NAME, __func__, "Key " << key.getId() << " found neither as a dependency nor an update in log entry "
					<< p.toHexString() << " by coordinator " << (int)coordinatorID_);
			return error::KEY_NOT_FOUND;
		}
	}
	returnValue = v;
	return error::SUCCESS;
	*/
}

void Coordinator::errorHandler(const ErrorType eType) {
	std::cerr << "Error Occured, type: " << eType << std::endl;
}
