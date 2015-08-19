/*
 *  FIFORequestBuffer.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "FIFORequestBuffer.hpp"

FIFORequestBuffer::FIFORequestBuffer() {}

void FIFORequestBuffer::add(req_ptr_t req) {
	std::unique_lock<std::mutex> locker(mu_);
	cond_.wait(locker, [this](){return buffer_.size() < size_;});
	buffer_.push_back(req);
	locker.unlock();
	cond_.notify_all();
	return;
}

req_ptr_t FIFORequestBuffer::remove() {
	std::unique_lock<std::mutex> locker(mu_);
	cond_.wait(locker, [this](){return buffer_.size() > 0;});
	req_ptr_t front = buffer_.front();
	buffer_.pop_front();
	locker.unlock();
	cond_.notify_all();
	return front;
}

FIFORequestBuffer::~FIFORequestBuffer() {}

