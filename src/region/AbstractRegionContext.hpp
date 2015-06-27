/*
 *  AbstractRegionContext.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 26, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef ABSTRACTREGIONCONTEXT_HPP_
#define ABSTRACTREGIONCONTEXT_HPP_

#include "../errors/Error.hpp"
#include <cstddef>	// for std::size_t

typedef error::ErrorType ErrorType;


//template <typename T>
class AbstractRegionContext {
public:
	virtual ErrorType read(void* destinationBuffer, const void* sourceBuffer, std::size_t length) = 0;
	virtual ErrorType write(const void* sourceBuffer, void* destinationBuffer, std::size_t length) = 0;

//	virtual ErrorType CAS(uintptr_t sourceBuffer, std::size_t length, T expectedValue, T swapValue, uintptr_t destinationBuffer) = 0;

	virtual ErrorType multicast() = 0;

	virtual ~AbstractRegionContext() = 0;
};

inline AbstractRegionContext::~AbstractRegionContext() {
	// empty
}

#endif /* ABSTRACTREGIONCONTEXT_HPP_ */
