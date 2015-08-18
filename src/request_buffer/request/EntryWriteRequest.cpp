/*
 *  EntryWriteRequest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 13, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "EntryWriteRequest.hpp"

EntryWriteRequest::EntryWriteRequest(
		err_promise_t						&prom,
		const size_t						memoryServerNum,
		const primitive::coordinator_num_t	cID,
		const LogEntry						&localEntry,
		const primitive::offset_t			remoteBufferOffset,
		const primitive::entry_size_t	length)
: Request(prom, memoryServerNum, Request::RequestType::WRITE, Request::RegionType::LOG),
  cID_(cID)
{
	params_.reset(new EntryWriteRequest::EntryWriteParameters(localEntry, remoteBufferOffset, length));
}

std::shared_ptr<EntryWriteRequest::EntryWriteParameters>& EntryWriteRequest::getParameters() {
	return params_;
}

primitive::coordinator_num_t EntryWriteRequest::getCoordinatorID() const {
	return cID_;
}

EntryWriteRequest::~EntryWriteRequest() {
	// TODO Auto-generated destructor stub
}
