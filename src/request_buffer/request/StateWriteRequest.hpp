/*
 *  StateRWRequest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 14, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef STATERWREQUEST_HPP_
#define STATERWREQUEST_HPP_

#include "Request.hpp"
#include "../../base_types/Pointer.hpp"
#include "../../base_types/EntryState.hpp"
#include <memory>

typedef std::shared_ptr<Pointer> pointer_ptr_t;


class StateWriteRequest: public Request {
public:
	struct StateWriteParameters {
		const Pointer		&entryPointer_;
		const EntryState::State	&state_;

		StateWriteParameters(const Pointer &entryPointer, const EntryState::State &state):
			entryPointer_(entryPointer),
			state_(state){}
	};

	StateWriteRequest(err_promise_t &, const size_t memoryServerNum, const Pointer &entryPointer, const EntryState::State &state);
	std::shared_ptr<StateWriteRequest::StateWriteParameters>& getParameters();

	~StateWriteRequest();

private:
	std::shared_ptr<StateWriteParameters>	params_;
};

#endif /* STATERWREQUEST_HPP_ */
