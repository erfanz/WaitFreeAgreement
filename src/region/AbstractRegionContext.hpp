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
#include <vector>

typedef error::ErrorType ErrorType;


template <typename T>
class AbstractRegionContext {
public:
	virtual ErrorType read(T* destinationBuffer, const primitive::offset_t sourceBufferOffset, const std::size_t length) = 0;
	virtual ErrorType write(const T* sourceBuffer, const primitive::offset_t destinationBufferOffset, const std::size_t length) = 0;
	virtual ErrorType CAS(T* expectedValue, const T &desiredValue, const primitive::offset_t sourceBufferOffset) = 0;
	virtual ~AbstractRegionContext() = 0;
};

// NOTE: A pure virtual destructor needs an implementation.
template <typename T>
inline AbstractRegionContext<T>::~AbstractRegionContext() {}

#endif /* ABSTRACTREGIONCONTEXT_HPP_ */
