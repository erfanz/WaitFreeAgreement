/*
 *  MemoryServerDispatcher.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef MEMORYREQUESTDISPATCHER_HPP_
#define MEMORYREQUESTDISPATCHER_HPP_

#include "Instance.hpp"
#include "HashBucket.hpp"
#include "LogJournal.hpp"
#include "../../request_buffer/RequestBuffer.hpp"
#include "../../request_buffer/request/Request.hpp"
#include "../../request_buffer/request/BucketReadRequest.hpp"
#include "../../request_buffer/request/CASRequest.hpp"
#include "../../request_buffer/request/EntryReadRequest.hpp"
#include "../../request_buffer/request/EntryWriteRequest.hpp"
#include "../../request_buffer/request/StateReadRequest.hpp"
#include "../../request_buffer/request/StateWriteRequest.hpp"
#include <vector>
#include <memory>	// std::shared_ptr

class MemoryRequestDispatcher {
private:
	std::shared_ptr<RequestBuffer> reqBufferPtr_;
	std::vector<Instance> instances_;

	ErrorType handleCASRequest(std::shared_ptr<CASRequest> &casPtr);
	ErrorType handleReadEntryRequest(std::shared_ptr<EntryReadRequest> &entryReadReqPtr);
	ErrorType handleWriteEntryRequest(std::shared_ptr<EntryWriteRequest> &entryWriteReqPtr);
	ErrorType handleReadBucketRequest(std::shared_ptr<BucketReadRequest> &bucketReqPtr);
	ErrorType handleReadStateRequest(std::shared_ptr<StateReadRequest> &stateReadReqPtr);
	ErrorType handleWriteStateRequest(std::shared_ptr<StateWriteRequest> &stateWriteReqPtr);

public:
	MemoryRequestDispatcher();
	std::shared_ptr<RequestBuffer> getRequestBuffer();
	void resetAllInstances();
	void run();
	~MemoryRequestDispatcher();
};

#endif /* MEMORYREQUESTDISPATCHER_HPP_ */
