/*
 *  Request.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include "../../errors/Error.hpp"
#include "../../base_types/PrimitiveTypes.hpp"
#include <future>         // std::promise, std::future

typedef error::ErrorType ErrorType;
typedef std::promise<ErrorType> err_promise_t;

class Request {
public:
	enum RequestType {
		READ,
		WRITE,
		CAS
	};

	enum RegionType {
		HASH,
		LOG,
		ENTRY_STATE
	};

	Request(err_promise_t &prom, const size_t memoryServerNum, const RequestType requestType, const RegionType regionType);
	const Request::RequestType& getRequestType() const;
	const Request::RegionType& getRegionType() const;
	size_t getMemoryServerNumber() const;
	void setProm(const ErrorType eType);
	virtual ~Request() = 0;

protected:
	err_promise_t		&prom_;
	const size_t		memoryServerNum_;
	const RequestType	requestType_;
	const RegionType	regionType_;
};

#endif /* REQUEST_HPP_ */
