/*
 *  CASRequest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef CASREQUEST_HPP_
#define CASREQUEST_HPP_

#include "Request.hpp"
#include "../../base_types/Pointer.hpp"
#include <memory>	// std::shared_ptr
#include <iostream>

class CASRequest: public Request {
public:

	struct CASParameters {
		const Pointer				&expectedHead_;
		const Pointer				&desiredHead_;
		const primitive::offset_t	offset_;
		Pointer						&actualHead_;

		CASParameters(const Pointer &expectedHead, const Pointer &desiredHead, const primitive::offset_t offset, Pointer &actualHead):
			expectedHead_(expectedHead),
			desiredHead_(desiredHead),
			offset_(offset),
			actualHead_(actualHead){}
	};

	CASRequest(err_promise_t &, const size_t memoryServerNum, const Pointer &expectedHead, const Pointer &desiredHead, const primitive::offset_t offset, Pointer &actualHead);
	std::shared_ptr<CASParameters>& getParameters();
	~CASRequest();

private:
	std::shared_ptr<CASParameters> params_;
};

#endif /* CASREQUEST_HPP_ */
