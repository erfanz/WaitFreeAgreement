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
#include "../../request_buffer/Request.hpp"
#include "../../request_buffer/ReadWriteRequest.hpp"
#include "../../request_buffer/CASRequest.hpp"


#include <cstdlib>	// for strtol


#define CLASS_NAME	"MSCtx"

//typedef std::shared_ptr<Request>  req_ptr_t;
//typedef std::shared_ptr<ReadWriteRequest> rw_ptr_t;
//typedef std::shared_ptr<CASRequest> cas_ptr_t;
//
//typedef Request::RequestType ReqType;
//typedef Request::RegionType RegType;
//
//MemoryServerContext::MemoryServerContext(const size_t memServerCtxID, bool isLocal)
//: memServerCtxID_(memServerCtxID),
//  isLocal_(isLocal) {
//	DEBUG_COUT(CLASS_NAME, __func__, "Memory Server Context " << (int)memServerCtxID_ << " created");
//}
//
//void MemoryServerContext::writeLogEntry(const primitive::coordinator_num_t cID, const LogEntry &entry, std::promise<ErrorType> &errorProm) {
//	std::ostringstream os;
//	entry.serialize(os);
//
//	const std::string& tmp = os.str();
//	const char* cstr = tmp.c_str();
//
//	// ErrorType eType = logJournals_[cID]->write(cstr, entry.getCurrentP().getOffset(), entry.getCurrentP().getLength());
//	//errorProm.set_value(eType);
//
//	req_ptr_t ptr(new ReadWriteRequest(
//			errorProm,
//			memServerCtxID_,
//			ReqType::WRITE,
//			RegType::LOG,
//			cID,
//			cstr,
//			entry.getCurrentP().getOffset(),
//			entry.getCurrentP().getLength()));
//
//	reqBufferPtr->add(ptr);
//
//	DEBUG_COUT(CLASS_NAME, __func__, "Write request for entry " << entry.getCurrentP().toHexString() << " placed on the queue");
//}
//
//void MemoryServerContext::readLogEntry(const Pointer &pointer, std::promise<ErrorType> &errorProm, LogEntry &entry) const {
//	char readBuffer[pointer.getLength()];
//	primitive::coordinator_num_t cID = pointer.getCoordinatorNum();
//
//	ErrorType eType = logJournals_[cID]->read(readBuffer, pointer.getOffset(), pointer.getLength());
//
//	if (eType == error::SUCCESS) {
//		std::string tempStr(readBuffer);
//		std::istringstream is(tempStr);
//		LogEntry::deserialize(is, entry);
//		DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString() << " read from log journal[" << (int)cID << "] from MS " << memServerCtxID_);
//	}
//
//	errorProm.set_value(eType);
//}
//
//void MemoryServerContext::readBucketHash(const size_t bucketID, std::promise<ErrorType> &errorProm, Pointer &pointer) const{
//	primitive::pointer_size_t	readBuffer[1];
//	primitive::offset_t			offset = (primitive::offset_t)(bucketID);
//
//	ErrorType eType = bucketHash_->read(readBuffer, offset, sizeof(primitive::pointer_size_t));
//	if (eType != error::SUCCESS) {
//		errorProm.set_value(eType);
//		return;
//	}
//
//	pointer = Pointer::makePointer(readBuffer[0]);
//
//	DEBUG_COUT(CLASS_NAME, __func__, "Bucket hash " << bucketID << " read from MS " << memServerCtxID_
//			<<  ". It points to " << pointer.toHexString());
//
//	if (pointer.isNull())
//		errorProm.set_value(error::KEY_NOT_FOUND);
//	else
//		errorProm.set_value(error::SUCCESS);
//}
//
//void MemoryServerContext::swapBucketHash(const size_t bucketID,
//		const Pointer &expectedHead,
//		const Pointer &newHead,
//		std::promise<ErrorType> &errorProm,
//		Pointer &actualHead) {
//
//	primitive::offset_t			offset		= (primitive::offset_t)(bucketID);
//	primitive::pointer_size_t	expected	= expectedHead.toULL();
//	primitive::pointer_size_t	desired		= newHead.toULL();
//
//	ErrorType eType = bucketHash_->CAS(&expected, desired, offset);
//	if (eType == error::SUCCESS){
//		// TODO: if we know for sure that (expected == expectedHead.toULL()),
//		// then instead of the following line, we can write actualCurrentHead = expectedHead;
//		actualHead = expectedHead;
//		DEBUG_COUT(CLASS_NAME, __func__, "CASing bucket hash " << bucketID << " on MS " << memServerCtxID_
//				<< " succeeded (expected: " << expectedHead.toHexString() << ", real: " << actualHead.toHexString() << ")");
//	}
//	else {
//		actualHead = Pointer::makePointer(expected);
//		DEBUG_COUT(CLASS_NAME, __func__, "CASing bucket hash " << bucketID << " on MS " << memServerCtxID_
//						<< " failed (expected: " << expectedHead.toHexString() << ", real: " << actualHead.toHexString() << ")");
//	}
//	errorProm.set_value(eType);
//}
//
///*
//ErrorType MemoryServerContext::swapMultipleBucketHash(const std::vector<Dependency> &dependencies, const Pointer &newHead, std::vector<Pointer> &actualCurrentHeads) {
//	std::vector<primitive::offset_t> offsets;
//	std::vector<primitive::pointer_size_t> expectedHeads;
//	primitive::pointer_size_t desired	= newHead.toULL();
//	std::vector<primitive::pointer_size_t> actualHeads;
//
//	for (size_t i = 0; i < dependencies.size(); i++) {
//		offsets.push_back((primitive::offset_t)dependencies.at(i).getBucketID());
//		expectedHeads.push_back(dependencies.at(i).getPointer().toULL());
//	}
//
//
//	ErrorType eType = bucketHash_->multiCAS(expectedHeads, desired, offsets, actualHeads);
//	for (size_t i = 0; i < actualHeads.size(); i++)
//	if (eType == error::SUCCESS){
//		// TODO: if we know for sure that (expected == expectedHead.toULL()),
//		// then instead of the following line, we can write actualCurrentHead = expectedHead;
//
//		actualCurrentHead = expectedHead;
//		DEBUG_COUT(CLASS_NAME, __func__, "CASing bucket hash " << bucketID
//				<< " succeeded (expected: " << expectedHead.toHexString() << ", real: " << actualCurrentHead.toHexString() << ")");
//	}
//	else {
//
//		actualCurrentHead = Pointer::makePointer(expected);
//		DEBUG_COUT(CLASS_NAME, __func__, "CASing bucket hash " << bucketID
//						<< " failed (expected: " << expectedHead.toHexString() << ", real: " << actualCurrentHead.toHexString() << ")");
//	}
//
//	return eType;
//}
//*/
//
//void MemoryServerContext::markState(const LogEntry &entry, const EntryState::State state, std::promise<ErrorType> &errorProm) {
//	const primitive::coordinator_num_t	coordinatorID	= entry.getCurrentP().getCoordinatorNum();
//	std::size_t 						writeLength		= 1;	// we only want to change one byte, which is the serialization flag
//	primitive::offset_t					offset			= (primitive::offset_t)(entry.getCurrentP().getOffset()
//			+ Pointer::getTotalSize() + 1);		// since we first store the pointer and a whitespace before the serialized flag.
//
//	const std::string& trueFlag = utilities::ToString<int>(state);
//	const char* cstr = trueFlag.c_str();
//
//	ErrorType eType = logJournals_[coordinatorID]->write(cstr, offset, writeLength);
//	errorProm.set_value(eType);
//}
//
//void MemoryServerContext::checkState(const LogEntry &entry, std::promise<ErrorType> &errorProm, EntryState::State &state) {
//	char readBuffer[1];	// the serialization flag is 1 or 0
//	const primitive::coordinator_num_t	coordinatorID	= entry.getCurrentP().getCoordinatorNum();
//	std::size_t 						readLength		= 1;	// we only want to change one byte, which is the serialization flag
//	primitive::offset_t					offset			= (primitive::offset_t)(entry.getCurrentP().getOffset()
//			+ Pointer::getTotalSize() + 1);		// since we first store the pointer and a whitespace before the serialized flag
//
//	ErrorType eType = logJournals_[coordinatorID]->read(readBuffer, offset, readLength);
//	if (eType == error::SUCCESS) {
//		char *pNext;
//		state = static_cast<EntryState::State>(strtoul (readBuffer, &pNext, 10));
//	}
//	errorProm.set_value(eType);
//}

