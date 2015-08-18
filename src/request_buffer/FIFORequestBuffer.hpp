/*
 *  FifoRequestBuffer.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef FIFOREQUESTBUFFER_HPP_
#define FIFOREQUESTBUFFER_HPP_

#include "RequestBuffer.hpp"
#include <deque>

class FIFORequestBuffer: public RequestBuffer {
private:
	std::deque<req_ptr_t> buffer_;
public:
	FIFORequestBuffer();
	void add(req_ptr_t req);
	req_ptr_t remove();
	~FIFORequestBuffer();
};

#endif /* FIFOREQUESTBUFFER_HPP_ */
