/*
 *  EntryStateRWRequest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 14, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "StateReadRequest.hpp"

StateReadRequest::StateReadRequest(
		err_promise_t			&prom,
		const size_t			memoryServerNum,
		const Pointer			entryPointer,
		EntryState::State		&state)
: Request(prom, memoryServerNum, Request::RequestType::READ, Request::RegionType::ENTRY_STATE)
{
	params_.reset(new StateReadRequest::StateReadParameters(entryPointer, state));
}

std::shared_ptr<StateReadRequest::StateReadParameters>& StateReadRequest::getParameters() {
	return params_;
}

StateReadRequest::~StateReadRequest() {
	// TODO Auto-generated destructor stub
}
