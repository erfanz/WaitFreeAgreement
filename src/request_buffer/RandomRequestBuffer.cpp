/*
 *  RandomRequestBuffer.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "RandomRequestBuffer.hpp"

RandomRequestBuffer::RandomRequestBuffer() {
	// TODO Auto-generated constructor stub
}

template<typename RandomGenerator>
deq_it_t RandomRequestBuffer::select_randomly(deq_it_t start, deq_it_t end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, (int)std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

deq_it_t RandomRequestBuffer::select_randomly(deq_it_t start, deq_it_t end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

void RandomRequestBuffer::add(req_ptr_t req) {
	std::unique_lock<std::mutex> locker(mu_);
	cond_.wait(locker, [this](){return buffer_.size() < size_;});
	buffer_.push_back(req);
	locker.unlock();
	cond_.notify_all();
	return;
}

req_ptr_t RandomRequestBuffer::remove() {
	std::unique_lock<std::mutex> locker(mu_);
	cond_.wait(locker, [this](){return buffer_.size() > 0;});
	deq_it_t randomIt  = select_randomly(buffer_.begin(), buffer_.end());
	req_ptr_t randomElement = *randomIt;
	buffer_.erase(randomIt);
	locker.unlock();
	cond_.notify_all();
	return randomElement;
}

RandomRequestBuffer::~RandomRequestBuffer() {
	// TODO Auto-generated destructor stub
}
