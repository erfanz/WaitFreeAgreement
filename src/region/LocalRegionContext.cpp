/*
 *  LocalRegionContext.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 26, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "LocalRegionContext.hpp"
#include <cstring>	// std::memcpy()

LocalRegionContext::LocalRegionContext(void* buffer) {
	this->buffer_ = buffer;

}


ErrorType LocalRegionContext::read(void* destinationBuffer, primitive::offset_t sourceBufferOffset, std::size_t length) {
	// we are doing byte offset, so it's safe to cast the (void *) to (char *)
	std::memcpy(destinationBuffer, ((char *)buffer_) + sourceBufferOffset, length);
	return error::SUCCESS;
}

ErrorType LocalRegionContext::write(const void* sourceBuffer, primitive::offset_t destinationBufferOffset, std::size_t length) {
	std::memcpy(((char *)buffer_) + destinationBufferOffset, sourceBuffer, length);
	return error::SUCCESS;
}

//template< class T >
//ErrorType LocalRegionContext::CAS(uintptr_t sourceBuffer, std::size_t length, T expectedValue, T swapValue, uintptr_t destinationBuffer) {
//	return error::SUCCESS;
//}

ErrorType LocalRegionContext::multicast() {
	return error::SUCCESS;
}

LocalRegionContext::~LocalRegionContext() {
	// TODO Auto-generated destructor stub
}

