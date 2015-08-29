/*
 *  RequestBuffer.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef REQUESTBUFFER_HPP_
#define REQUESTBUFFER_HPP_

#include "request/Request.hpp"
#include "../config.hpp"
#include <mutex>
#include <condition_variable>
#include <memory>	// std::shared_ptr

typedef std::shared_ptr<Request> req_ptr_t;

class RequestBuffer {
protected:
	std::mutex mu_;
	std::condition_variable cond_;
	const size_t size_ = config::REQUEST_QUEUE_SIZE;

public:
	virtual void add(req_ptr_t req) = 0;
	virtual req_ptr_t remove() = 0;
	virtual ~RequestBuffer() = 0;
};

#endif /* REQUESTBUFFER_HPP_ */
