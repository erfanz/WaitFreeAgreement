/*
 *  LIFORequestBuffer.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "LIFORequestBuffer.hpp"

LIFORequestBuffer::LIFORequestBuffer() {}

void LIFORequestBuffer::add(req_ptr_t req) {
	std::unique_lock<std::mutex> locker(mu_);
	cond_.wait(locker, [this](){return buffer_.size() < size_;});
	buffer_.push_back(req);
	locker.unlock();
	cond_.notify_all();
	return;
}

req_ptr_t LIFORequestBuffer::remove() {
	std::unique_lock<std::mutex> locker(mu_);
	cond_.wait(locker, [this](){return buffer_.size() > 0;});
	req_ptr_t back = buffer_.back();
	buffer_.pop_back();
	locker.unlock();
	cond_.notify_all();
	return back;
}

LIFORequestBuffer::~LIFORequestBuffer() {
	// TODO Auto-generated destructor stub
}

