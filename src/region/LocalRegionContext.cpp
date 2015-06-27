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

LocalRegionContext::LocalRegionContext() {
	// TODO Auto-generated constructor stub
}


ErrorType LocalRegionContext::read(void* destinationBuffer, const void* sourceBuffer, std::size_t length) {
	std::memcpy(destinationBuffer, sourceBuffer, length);
	return error::SUCCESS;
}

ErrorType LocalRegionContext::write(const void* sourceBuffer, void* destinationBuffer, std::size_t length) {
	std::memcpy(destinationBuffer, sourceBuffer, length);
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

