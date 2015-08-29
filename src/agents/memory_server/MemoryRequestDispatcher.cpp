/*
 *  MemoryRequestDispatcher.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "MemoryRequestDispatcher.hpp"
#include "../../request_buffer/LIFORequestBuffer.hpp"
#include "../../request_buffer/FIFORequestBuffer.hpp"
#include "../../request_buffer/RandomRequestBuffer.hpp"
#include "../../errors/Error.hpp"
#include "../../util/utils.hpp"
#include <memory>	// std::shared_ptr
#include <cstdlib>	// for strtol
#include <chrono>	// std::chrono::seconds

#define CLASS_NAME	"MSDispatcher"

typedef error::ErrorType ErrorType;

MemoryRequestDispatcher::MemoryRequestDispatcher()
: replicas_(config::MEMORY_SERVER_CNT){
	DEBUG_COUT (CLASS_NAME, __func__, "MemoryRequestDispatcher created!");
	//reqBufferPtr_.reset(new LIFORequestBuffer());
	reqBufferPtr_.reset(new RandomRequestBuffer());
}

std::shared_ptr<RequestBuffer> MemoryRequestDispatcher::getRequestBuffer(){
	return reqBufferPtr_;
}

void MemoryRequestDispatcher::resetAllInstances() {
	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++)
		replicas_.at(i).resetMemoryRegions();
}

void MemoryRequestDispatcher::run() {
	ErrorType eType;
	while (true) {
		//std::this_thread::sleep_for(std::chrono::seconds(1));

		std::shared_ptr<Request> req = reqBufferPtr_->remove();

		if (req->getRequestType() == Request::RequestType::CAS) {
			DEBUG_COUT(CLASS_NAME, __func__, "Received CAS request");

			std::shared_ptr<CASRequest> casReqPtr = std::dynamic_pointer_cast<CASRequest> (req);
			eType = handleCASRequest(casReqPtr);
		}
		else if (req->getRegionType() == Request::RegionType::LOG) {
			if (req->getRequestType() == Request::RequestType::READ) {
				DEBUG_COUT(CLASS_NAME, __func__, "Received Log Read request");

				std::shared_ptr<EntryReadRequest> entryReadReqPtr = std::dynamic_pointer_cast<EntryReadRequest> (req);
				eType = handleReadEntryRequest(entryReadReqPtr);
			}
			else {	// entryReqPtr->getRequestType() == Request::RequestType::WRITE
				DEBUG_COUT(CLASS_NAME, __func__, "Received Log Write request");

				std::shared_ptr<EntryWriteRequest> entryWriteReqPtr = std::dynamic_pointer_cast<EntryWriteRequest> (req);
				eType = handleWriteEntryRequest(entryWriteReqPtr);
			}
		}
		else if (req->getRegionType() == Request::RegionType::HASH) {
			if (req->getRequestType() == Request::RequestType::READ) {
				DEBUG_COUT(CLASS_NAME, __func__, "Received Bucket Read request");

				std::shared_ptr<BucketReadRequest> bucketReadReqPtr = std::dynamic_pointer_cast<BucketReadRequest> (req);
				eType = handleReadBucketRequest(bucketReadReqPtr);
			}
			else  	// bucketReqPtr->getRequestType() == Request::RequestType::WRITE, which is an error
				eType = error::UNKNOWN_REQUEST_TYPE;
		}
		else if (req->getRegionType() == Request::RegionType::ENTRY_STATE) {
			if (req->getRequestType() == Request::RequestType::READ) {
				DEBUG_COUT(CLASS_NAME, __func__, "Received State Read request");

				std::shared_ptr<StateReadRequest> stateReadReqPtr = std::dynamic_pointer_cast<StateReadRequest> (req);
				eType = handleReadStateRequest(stateReadReqPtr);
			}
			else {
				DEBUG_COUT(CLASS_NAME, __func__, "Received State Write request");

				std::shared_ptr<StateWriteRequest> stateWriteReqPtr = std::dynamic_pointer_cast<StateWriteRequest> (req);
				eType = handleWriteStateRequest(stateWriteReqPtr);
			}
		}
		else {
			DEBUG_CERR(CLASS_NAME, __func__, "Received request with unknown type");
			eType = error::UNKNOWN_REQUEST_TYPE;
		}

		// We finally set the std::promise variable
		req->setProm(eType);
	}
}

ErrorType MemoryRequestDispatcher::handleCASRequest(std::shared_ptr<CASRequest> &casReqPtr) {
	ErrorType eType;
	size_t msNum = casReqPtr->getMemoryServerNumber();
	std::shared_ptr<CASRequest::CASParameters> paramPtr = casReqPtr->getParameters();
	primitive::pointer_size_t	expected	= paramPtr->expectedHead_.toULL();
	primitive::pointer_size_t	desired		= paramPtr->desiredHead_.toULL();
	size_t bucketID = paramPtr->offset_;

	eType = replicas_.at(msNum).getHashRegion()->CAS(&expected, desired, paramPtr->offset_);

	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "CASing bucket " << bucketID << " on MS " << msNum << " succeeded (expected: "
				<< paramPtr->expectedHead_.toHexString() << ", changed to: " << paramPtr->desiredHead_.toHexString() << ")");
	else {
		const Pointer& actualHead = Pointer::makePointer(expected);
		DEBUG_COUT(CLASS_NAME, __func__, "CASing bucket " << bucketID << " on MS " << msNum << " failed (expected: "
				<< paramPtr->expectedHead_.toHexString() << ", real head: " << actualHead.toHexString() << ")");
	}
	return eType;
}

ErrorType MemoryRequestDispatcher::handleReadEntryRequest(std::shared_ptr<EntryReadRequest> &entryReadReqPtr){
	ErrorType eType;
	size_t msNum = entryReadReqPtr->getMemoryServerNumber();
	primitive::coordinator_num_t cID = entryReadReqPtr->getCoordinatorID();
	std::shared_ptr<EntryReadRequest::EntryReadParameters> paramPtr = entryReadReqPtr->getParameters();
	LogEntry &entry = paramPtr->localEntry_;
	char readBuffer[paramPtr->length_];

	eType = replicas_.at(msNum).getLogRegion(cID)->read(readBuffer, paramPtr->remoteBufferOffset_, paramPtr->length_);

	if (eType == error::SUCCESS) {
		std::string tempStr(readBuffer, paramPtr->length_);
		std::istringstream is(tempStr);
		LogEntry::deserialize(is, entry);
		DEBUG_COUT(CLASS_NAME, __func__, "Log entry " << entry.getCurrentP().toHexString() << " read from log journal[" << (int)cID << "] from MS " << msNum);
	}
	else
		DEBUG_COUT(CLASS_NAME, __func__, "Failure in reading Log entry " << entry.getCurrentP().toHexString() << " from log journal[" << (int)cID << "] from MS " << msNum);

	return eType;
}

ErrorType MemoryRequestDispatcher::handleWriteEntryRequest(std::shared_ptr<EntryWriteRequest> &entryReqPtr){
	ErrorType eType;
	size_t msNum = entryReqPtr->getMemoryServerNumber();
	primitive::coordinator_num_t cID = entryReqPtr->getCoordinatorID();
	std::shared_ptr<EntryWriteRequest::EntryWriteParameters> paramPtr = entryReqPtr->getParameters();
	const LogEntry &entry = paramPtr->localEntry_;

	std::ostringstream os;
	entry.serialize(os);

	const std::string& tmp = os.str();
	const char* cstr = tmp.c_str();

	eType = replicas_.at(msNum).getLogRegion(cID)->write(cstr, paramPtr->remoteBufferOffset_, paramPtr->length_);

	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "Log Entry " << entry.getCurrentP().toHexString() << " written on log journal[" << (int)cID << "] on MS " << msNum);
	else
		DEBUG_COUT(CLASS_NAME, __func__, "Failure in writing log Entry " << entry.getCurrentP().toHexString() << " on log journal[" << (int)cID << "] on MS " << msNum);

	return eType;
}

ErrorType MemoryRequestDispatcher::handleReadBucketRequest(std::shared_ptr<BucketReadRequest> &bucketReadReqPtr) {
	ErrorType eType;
	size_t msNum = bucketReadReqPtr->getMemoryServerNumber();
	std::shared_ptr<BucketReadRequest::BucketReadParameters> paramPtr = bucketReadReqPtr->getParameters();
	size_t bucketID = paramPtr->remoteBufferOffset_;
	Pointer &pointer = paramPtr->localPointer_;
	primitive::pointer_size_t	readBuffer[1];

	eType = replicas_.at(msNum).getHashRegion()->read(readBuffer, paramPtr->remoteBufferOffset_, sizeof(primitive::pointer_size_t));
	if (eType != error::SUCCESS) {
		DEBUG_COUT(CLASS_NAME, __func__, "Failure in reading bucket " << bucketID << " from MS " << msNum);
		return eType;
	}

	pointer = Pointer::makePointer(readBuffer[0]);

	DEBUG_COUT(CLASS_NAME, __func__, "Bucket " << bucketID << " read from MS " << msNum <<  " and is pointing to " << pointer.toHexString());

	// TODO: what if the bucket is empty
	// if (pointer.isNull())
	//	eType = error::KEY_NOT_FOUND;

	return eType;
}

ErrorType MemoryRequestDispatcher::handleReadStateRequest(std::shared_ptr<StateReadRequest> &stateReadReqPtr) {
	ErrorType eType;
	size_t msNum = stateReadReqPtr->getMemoryServerNumber();
	std::shared_ptr<StateReadRequest::StateReadParameters> paramPtr = stateReadReqPtr->getParameters();
	const Pointer &entryPointer = paramPtr->entryPointer_;
	primitive::coordinator_num_t cID = entryPointer.getCoordinatorNum();

	char readBuffer[1];	// the serialization flag is 1 digit only
	std::size_t 						readLength		= 1;	// we only want to change one byte, which is the serialization flag
	primitive::offset_t					offset			= (primitive::offset_t)(entryPointer.getOffset()
				+ Pointer::getTotalSize() + 1);		// since we first store the pointer and a whitespace before the serialized flag

	eType = replicas_.at(msNum).getLogRegion(cID)->read(readBuffer, offset, readLength);
	if (eType == error::SUCCESS) {
		char *pNext;
		paramPtr->state_ = static_cast<EntryState::State>(strtoul (readBuffer, &pNext, 10));
	}

	return eType;
}

ErrorType MemoryRequestDispatcher::handleWriteStateRequest(std::shared_ptr<StateWriteRequest> &stateWriteReqPtr) {
	ErrorType eType;
	size_t msNum = stateWriteReqPtr->getMemoryServerNumber();
	std::shared_ptr<StateWriteRequest::StateWriteParameters> paramPtr = stateWriteReqPtr->getParameters();
	const Pointer &entryPointer = paramPtr->entryPointer_;
	primitive::coordinator_num_t cID = entryPointer.getCoordinatorNum();

	std::size_t 						writeLength		= 1;	// we only want to change one byte, which is the serialization flag
	primitive::offset_t					offset			= (primitive::offset_t)(entryPointer.getOffset()
			+ Pointer::getTotalSize() + 1);		// since we first store the pointer and a whitespace before the serialized flag.

	const std::string& state = utilities::ToString<int>(paramPtr->state_);
	const char* cstr = state.c_str();

	eType = replicas_.at(msNum).getLogRegion(cID)->write(cstr, offset, writeLength);
	return eType;
}

MemoryRequestDispatcher::~MemoryRequestDispatcher() {
	DEBUG_COUT (CLASS_NAME, __func__, "MemoryRequestDispatcher destroyed!");
}
