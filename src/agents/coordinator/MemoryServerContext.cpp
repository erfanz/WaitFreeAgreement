/*
 *  MemoryServerContext.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "MemoryServerContext.hpp"
#include "../../base_types/PrimitiveTypes.hpp"
#include "../../base_types/Pointer.hpp"
#include "../../config.hpp"
#include "../../util/utils.hpp"	// for toString();

#define CLASS_NAME	"MSCtx"


MemoryServerContext::MemoryServerContext(MemoryServer &memoryServer, bool isLocal){
	std::atomic<uint64_t>*	bucketHash;
	std::atomic<uint64_t>*	bucketValid;
	std::atomic<char>**	logJournal;
	memoryServer.getMemoryHandlers(&bucketValid, &bucketHash, &logJournal);

	this->isLocal_ = isLocal;
	if (isLocal_) {
		bucketHash_ = new LocalRegionContext<uint64_t>(bucketHash);
		bucketValid_ = new LocalRegionContext<uint64_t>(bucketValid);
		logJournals_ = new LocalRegionContext<char>* [config::COORDINATOR_CNT];

		for (size_t i = 0; i < config::COORDINATOR_CNT; i++) {
			logJournals_[i] = new LocalRegionContext<char>(logJournal[i]);
		}
	}
	DEBUG_COUT(CLASS_NAME, __func__, "Memory Server Context created");
}

ErrorType MemoryServerContext::writeLogEntry(const primitive::coordinator_num_t cID, const LogEntry &entry) {
	std::ostringstream os;
	entry.serialize(os);

	const std::string& tmp = os.str();
	const char* cstr = tmp.c_str();

	logJournals_[cID]->write(cstr, entry.getCurrentP().getOffset(), entry.getCurrentP().getLength());

	DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString() << " written on log journal[" << (int)cID << "]");
	return error::SUCCESS;
}

ErrorType MemoryServerContext::readLogEntry(const Pointer &pointer, LogEntry &entry) {
	char readBuffer[pointer.getLength()];
	primitive::coordinator_num_t cID = pointer.getCoordinatorNum();

	logJournals_[cID]->read(readBuffer, pointer.getOffset(), pointer.getLength());
	std::string tempStr(readBuffer);
	std::istringstream is(tempStr);
	LogEntry::deserialize(is, entry);

	DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString() << " read from log journal[" << (int)cID << "]");
	return error::SUCCESS;
}

ErrorType MemoryServerContext::readBucketHash(const HashMaker &hashedKey, Pointer &pointer){
	primitive::pointer_size_t	readBuffer[1];
	primitive::offset_t			offset = (primitive::offset_t)(hashedKey.getHashed());

	bucketHash_->read(readBuffer, offset, sizeof(primitive::pointer_size_t));

	pointer = Pointer::makePointer(readBuffer[0]);

	DEBUG_COUT(CLASS_NAME, __func__, "Bucket hash " << hashedKey.getHashed() << " read. It points to " << pointer.toHexString());

	if (pointer.isNull())
		return error::KEY_NOT_FOUND;
	else
		return error::SUCCESS;
}

ErrorType MemoryServerContext::swapBucketHash(const size_t bucketID, const Pointer &expectedHead, const Pointer &newHead, Pointer &actualCurrentHead) {
	primitive::offset_t offset = (primitive::offset_t)(bucketID);
	primitive::pointer_size_t expected	= expectedHead.toULL();
	primitive::pointer_size_t desired	= newHead.toULL();

	ErrorType eType = bucketHash_->CAS(&expected, desired, offset);
	if (eType == error::SUCCESS){
		// TODO: if we know for sure that (expected == expectedHead.toULL()),
		// then instead of the following line, we can write actualCurrentHead = expectedHead;
		actualCurrentHead = expectedHead;
		DEBUG_COUT(CLASS_NAME, __func__, "CASing bucket hash " << bucketID
				<< " succeeded (expected: " << expectedHead.toHexString() << ", real: " << actualCurrentHead.toHexString() << ")");
	}
	else {

		actualCurrentHead = Pointer::makePointer(expected);
		DEBUG_COUT(CLASS_NAME, __func__, "CASing bucket hash " << bucketID
						<< " failed (expected: " << expectedHead.toHexString() << ", real: " << actualCurrentHead.toHexString() << ")");
	}

	return eType;
}

ErrorType MemoryServerContext::markSerialized(const primitive::coordinator_num_t cID, const LogEntry &entry) {
	std::size_t 		writeLength	= 1;	// we only want to change one byte, which is the serialization flag
	primitive::offset_t	offset		= (primitive::offset_t)(entry.getCurrentP().getOffset()
			+ Pointer::getTotalSize() + 1);		// since we first store the pointer and a whitespace before the serialized flag.

	const std::string& trueFlag = utilities::ToString<bool>(true);
	const char* cstr = trueFlag.c_str();

	logJournals_[cID]->write(cstr, offset, writeLength);

	DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString() << " marked serialized on log journal[" << (int)cID << "]");
	return error::SUCCESS;
}
