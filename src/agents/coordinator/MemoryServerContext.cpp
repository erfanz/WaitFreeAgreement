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

MemoryServerContext::MemoryServerContext(MemoryServer &memoryServer, bool isLocal){
	std::atomic<uint64_t>*	bucketHash;
	std::atomic<uint64_t>*	bucketValid;
	std::atomic<char>**	logJournal;
	memoryServer.getMemoryHandlers(&bucketValid, &bucketHash, &logJournal);

	//std::atomic<uint64_t>*	bucketHash2;
	//LocalRegionContext<uint64_t>* bucketHash2_ = new LocalRegionContext<uint64_t>(bucketHash2);

	this->isLocal_ = isLocal;
	if (isLocal_) {
		bucketHash_ = new LocalRegionContext<uint64_t>(bucketHash);
		bucketValid_ = new LocalRegionContext<uint64_t>(bucketValid);
		logJournals_ = new LocalRegionContext<char>* [config::COORDINATOR_CNT];

		for (size_t i = 0; i < config::COORDINATOR_CNT; i++) {
			logJournals_[i] = new LocalRegionContext<char>(logJournal[i]);
		}
	}
}
//
//bool MemoryServerContext::isKeyValid (const uint64_t hashedKey, const Key key) const{
//	if (isLocal){
//		SCN scn(memoryServer.keyValid);
//		// TODO
//		return true;
//
//	}
//	else {
//		// TODO
//		return false;
//	}
//}
//
//int MemoryServerContext::getLastMissedSCN(const HashMaker hashedKey, SCN &outputScn) const{
//	outputScn = memoryServer.keyValid[hashedKey.getHashed()];
//	return 0;
//}
//
//int MemoryServerContext::findItemByVersion(HashMaker hashedKey, Key key, SCN scn, TID tid, LogEntry &output_entry) const{
//	Pointer p;
//	getKeyHash(hashedKey.getHashed(), Pointer &p);
//	Pointer p = p.makePointer(memoryServer.keyHash[hashedKey.getHashed()]);
//
//	LogEntry *entry = fetchLogEntry(p.coordinatorNum, p.offset, p.length);
//	if (entry->key.isEqual(key)) {
//		// this is the item we are looking for
//		return entry;
//	}
//	return 0;
//}
//
//LogEntry* MemoryServerContext::fetchLogEntry(uint8_t coordinatorNum, uint32_t offset, uint16_t length) const {
//	LogEntry *entry = new LogEntry();
//
//
//}
//
//int getKeyHash(HashMaker &, Pointer &p);


ErrorType MemoryServerContext::writeLogEntry(const primitive::coordinator_num_t cID, const LogEntry &entry) {
	std::ostringstream os;
	entry.serialize(os);

	const std::string& tmp = os.str();
	const char* cstr = tmp.c_str();

	logJournals_[cID]->write(cstr, entry.getCurrentP().getOffset(), entry.getCurrentP().getLength());
	return error::SUCCESS;
}

ErrorType MemoryServerContext::readLogEntry(const Pointer &pointer, LogEntry &entry) {
	char readBuffer[pointer.getLength()];
	primitive::coordinator_num_t cID = pointer.getCoordinatorNum();

	logJournals_[cID]->read(readBuffer, pointer.getOffset(), pointer.getLength());
	std::string tempStr(readBuffer);
	std::istringstream is(tempStr);
	LogEntry::deserialize(is, entry);
	return error::SUCCESS;
}

ErrorType MemoryServerContext::readBucketHash(const HashMaker &hashedKey, Pointer &pointer){
	primitive::pointer_size_t	readBuffer[1];
	primitive::offset_t			offset = (primitive::offset_t)(hashedKey.getHashed());

	std::cout << "pointer: " << readBuffer[0] << std::endl;

	bucketHash_->read(readBuffer, offset, sizeof(primitive::pointer_size_t));

	pointer = Pointer::makePointer(readBuffer[0]);

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
	}
	else
		actualCurrentHead = Pointer::makePointer(expected);

	return eType;
}

ErrorType MemoryServerContext::markSerialized(const primitive::coordinator_num_t cID, const LogEntry &entry) {
	std::size_t 		writeLength	= 1;	// we only want to change one byte, which is the serialization flag
	primitive::offset_t	offset		= (primitive::offset_t)(entry.getCurrentP().getOffset()
			+ Pointer::getTotalSize() + 1);		// since we first store the pointer and a whitespace before the serialized flag.

	const std::string& trueFlag = utilities::ToString<bool>(true);
	const char* cstr = trueFlag.c_str();

	logJournals_[cID]->write(cstr, offset, writeLength);
	return error::SUCCESS;
}
