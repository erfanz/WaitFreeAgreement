/*
 *  ReadWriteRequest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "BucketReadRequest.hpp"
#include "../../util/utils.hpp"

#define CLASS_NAME	"BucketReadRequest"

BucketReadRequest::BucketReadRequest(
		err_promise_t				&prom,
		const size_t				memoryServerNum,
		Pointer						&localPointer,
		const primitive::offset_t	remoteBufferOffset)
: Request(prom, memoryServerNum, Request::RequestType::READ, Request::RegionType::HASH)
{
	params_.reset(new BucketReadRequest::BucketReadParameters(localPointer, remoteBufferOffset));
}

std::shared_ptr<BucketReadRequest::BucketReadParameters>& BucketReadRequest::getParameters() {
	return params_;
}

BucketReadRequest::~BucketReadRequest() {
	// DEBUG_COUT(CLASS_NAME, __func__, "Destroyed!");
}
