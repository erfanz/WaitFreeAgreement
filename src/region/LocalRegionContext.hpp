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
#include <cstring>	// std::memcpy()
#include <atomic>

template <typename T>
class LocalRegionContext: public AbstractRegionContext<T> {
private:
	std::atomic<T>* buffer_;

public:
	LocalRegionContext(std::atomic<T>* buffer);
	ErrorType read(T* destinationBuffer, const primitive::offset_t sourceBufferOffset, const std::size_t length);
	ErrorType write(const T* sourceBuffer, const primitive::offset_t destinationBufferOffset, const std::size_t length);
	ErrorType CAS(T* expectedValue, const T &desiredValue, const primitive::offset_t sourceBufferOffset);
	ErrorType multiCAS(std::vector<T*> expectedValues, const T &desiredValue, const std::vector<primitive::offset_t> sourceBufferOffsets);
	~LocalRegionContext();
};


/* Note that template classes must be implemented in the header file, since the compiler
 * needs to have access to the implementation of the methods when instantiating the template class at compile time.
 * For more information, please refer to:
 * http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
 */

template <typename T>
LocalRegionContext<T>::LocalRegionContext(std::atomic<T>* buffer) {
	this->buffer_ = buffer;
}

template <typename T>
ErrorType LocalRegionContext<T>::read(T* destinationBuffer, const primitive::offset_t sourceBufferOffset, const std::size_t length) {
	std::memcpy(destinationBuffer, buffer_ + sourceBufferOffset, length);
	return error::SUCCESS;
}

template <typename T>
ErrorType LocalRegionContext<T>::write(const T* sourceBuffer, const primitive::offset_t destinationBufferOffset, const std::size_t length) {
	std::memcpy(buffer_ + destinationBufferOffset, sourceBuffer, length);
	return error::SUCCESS;
}

template <typename T>
ErrorType LocalRegionContext<T>::CAS(T* expectedValue, const T &desiredValue, const primitive::offset_t sourceBufferOffset) {
	std::atomic<T>* comparingObj = buffer_ + sourceBufferOffset;
	if (atomic_compare_exchange_strong(comparingObj, expectedValue, desiredValue) == true)
		return error::SUCCESS;
	else
		return error::CAS_FAILED;
}

template <typename T>
ErrorType LocalRegionContext<T>::multiCAS(std::vector<T*> expectedValues, const T &desiredValue, const std::vector<primitive::offset_t> sourceBufferOffsets) {
	return error::SUCCESS;
}

template <typename T>
LocalRegionContext<T>::~LocalRegionContext() {
	// TODO Auto-generated destructor stub
}

#endif /* LOCALREGIONCONTEXT_HPP_ */
