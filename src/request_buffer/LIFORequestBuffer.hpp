/*
 *  LIFORequestBuffer.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef LIFOREQUESTBUFFER_HPP_
#define LIFOREQUESTBUFFER_HPP_

#include "RequestBuffer.hpp"
#include <deque>

class LIFORequestBuffer : public RequestBuffer{
private:
	std::deque<req_ptr_t> buffer_;
public:
	LIFORequestBuffer();
	void add(req_ptr_t req);
	req_ptr_t remove();
	~LIFORequestBuffer();
};

#endif /* LIFOREQUESTBUFFER_HPP_ */
