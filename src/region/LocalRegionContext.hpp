/*
 *  LocalRegionContext.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 26, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef LOCALREGIONCONTEXT_HPP_
#define LOCALREGIONCONTEXT_HPP_

#include "AbstractRegionContext.hpp"

class LocalRegionContext: public AbstractRegionContext {
public:
	LocalRegionContext();

	ErrorType read(void* destinationBuffer, const void* sourceBuffer, std::size_t length);
	ErrorType write(const void* sourceBuffer, void* destinationBuffer, std::size_t length);

//	template< class T >
//	ErrorType CAS(uintptr_t sourceBuffer, std::size_t length, T expectedValue, T swapValue, uintptr_t destinationBuffer);

	ErrorType multicast();

	~LocalRegionContext();


};

#endif /* LOCALREGIONCONTEXT_HPP_ */
