/*
 *  StateRWRequest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 14, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef STATEREADREQUEST_HPP_
#define STATEREADREQUEST_HPP_

#include "Request.hpp"
#include "../../base_types/Pointer.hpp"
#include "../../base_types/EntryState.hpp"
#include <memory>

typedef std::shared_ptr<Pointer> pointer_ptr_t;


class StateReadRequest: public Request {
public:
	struct StateReadParameters {
		const Pointer		&entryPointer_;
		EntryState::State	&state_;

		StateReadParameters(const Pointer &entryPointer, EntryState::State &state):
			entryPointer_(entryPointer),
			state_(state){}
	};

	StateReadRequest(err_promise_t &, const size_t memoryServerNum, const Pointer &entryPointer, EntryState::State &state);
	std::shared_ptr<StateReadRequest::StateReadParameters>& getParameters();

	~StateReadRequest();

private:
	std::shared_ptr<StateReadParameters>	params_;
};

#endif /* STATEREADREQUEST_HPP_ */
