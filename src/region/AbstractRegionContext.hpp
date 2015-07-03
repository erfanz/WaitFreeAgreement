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
#include "../base_types/PrimitiveTypes.hpp"
#include <cstddef>	// for std::size_t

typedef error::ErrorType ErrorType;


template <typename T>
class AbstractRegionContext {
public:
	virtual ErrorType read(T* destinationBuffer, primitive::offset_t sourceBufferOffset, std::size_t length) = 0;
	virtual ErrorType write(const T* sourceBuffer, primitive::offset_t destinationBufferOffset, std::size_t length) = 0;

//	virtual ErrorType CAS(uintptr_t sourceBuffer, std::size_t length, T expectedValue, T swapValue, uintptr_t destinationBuffer) = 0;

	virtual ErrorType multicast() = 0;

	virtual ~AbstractRegionContext() = 0;
};

template <typename T>
inline AbstractRegionContext<T>::~AbstractRegionContext() {
	// empty
}

#endif /* ABSTRACTREGIONCONTEXT_HPP_ */
