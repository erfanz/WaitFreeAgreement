/*
 *  Request.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Request.hpp"

Request::Request(
		err_promise_t 		&prom,
		const size_t		memoryServerNum,
		const RequestType	requestType,
		const RegionType	regionType)
: prom_(prom),
  memoryServerNum_(memoryServerNum),
  requestType_(requestType),
  regionType_(regionType) {
}


const typename Request::RequestType& Request::getRequestType() const {
	return requestType_;
}

const typename Request::RegionType& Request::getRegionType() const {
	return regionType_;
}

size_t Request::getMemoryServerNumber() const {
	return memoryServerNum_;
}


void Request::setProm(const ErrorType eType) {
	prom_.set_value(eType);
}

// NOTE: A pure virtual destructor needs an implementation.
Request::~Request() { }
