/*
 *  BucketReadRequest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef BUCKETRWREQUEST_HPP_
#define BUCKETRWREQUEST_HPP_

#include "Request.hpp"
#include "../../base_types/PrimitiveTypes.hpp"
#include "../../base_types/Pointer.hpp"
#include "memory"	// std::shared_ptr

typedef std::shared_ptr<Pointer> pointer_ptr_t;

class BucketReadRequest: public Request {
public:
	struct BucketReadParameters {
		Pointer						&localPointer_;
		const primitive::offset_t	remoteBufferOffset_;

		BucketReadParameters(Pointer &localPointer, const primitive::offset_t remoteBufferOffset):
			localPointer_(localPointer),
			remoteBufferOffset_(remoteBufferOffset){}
	};

	BucketReadRequest(err_promise_t &, const size_t memoryServerNum, Pointer &localPointer, const primitive::offset_t remoteBufferOffset);

	std::shared_ptr<BucketReadParameters>& getParameters();
	~BucketReadRequest();

private:
	std::shared_ptr<BucketReadParameters>	params_;

};

#endif /* BUCKETRWREQUEST_HPP_ */
