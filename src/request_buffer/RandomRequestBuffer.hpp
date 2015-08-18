/*
 *  RandomRequestBuffer.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef RANDOMREQUESTBUFFER_HPP_
#define RANDOMREQUESTBUFFER_HPP_

#include "RequestBuffer.hpp"
#include <deque>
#include  <random>
#include  <iterator>

typedef std::deque<req_ptr_t>::iterator deq_it_t;

class RandomRequestBuffer: public RequestBuffer {
private:
	std::deque<req_ptr_t> buffer_;

	template<typename RandomGenerator> deq_it_t select_randomly(deq_it_t start, deq_it_t end, RandomGenerator& g);
	deq_it_t select_randomly(deq_it_t start, deq_it_t end);

public:
	RandomRequestBuffer();
	void add(req_ptr_t req);
	req_ptr_t remove();
	~RandomRequestBuffer();
};

#endif /* RANDOMREQUESTBUFFER_HPP_ */
