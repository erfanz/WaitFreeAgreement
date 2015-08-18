/*
 *  EntryRWRequest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 13, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "EntryReadRequest.hpp"

EntryReadRequest::EntryReadRequest(
		err_promise_t						&prom,
		const size_t						memoryServerNum,
		const primitive::coordinator_num_t	cID,
		LogEntry							&localEntry,
		const primitive::offset_t			remoteBufferOffset,
		const primitive::entry_size_t	length)
: Request(prom, memoryServerNum, Request::RequestType::READ, Request::RegionType::LOG),
  cID_(cID)
{
	params_.reset(new EntryReadRequest::EntryReadParameters(localEntry, remoteBufferOffset, length));
}

std::shared_ptr<EntryReadRequest::EntryReadParameters>& EntryReadRequest::getParameters() {
	return params_;
}

primitive::coordinator_num_t EntryReadRequest::getCoordinatorID() const {
	return cID_;
}

EntryReadRequest::~EntryReadRequest() {
	// TODO Auto-generated destructor stub
}

