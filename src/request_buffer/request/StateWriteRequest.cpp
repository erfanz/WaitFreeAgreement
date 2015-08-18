/*
 *  StateRWRequest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 14, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "StateWriteRequest.hpp"

StateWriteRequest::StateWriteRequest(
		err_promise_t			&prom,
		const size_t			memoryServerNum,
		const Pointer			&entryPointer,
		const EntryState::State	&state)
: Request(prom, memoryServerNum, Request::RequestType::WRITE, Request::RegionType::ENTRY_STATE)
{
	params_.reset(new StateWriteRequest::StateWriteParameters(entryPointer, state));
}

std::shared_ptr<StateWriteRequest::StateWriteParameters>& StateWriteRequest::getParameters() {
	return params_;
}

StateWriteRequest::~StateWriteRequest() {
	// TODO Auto-generated destructor stub
}
