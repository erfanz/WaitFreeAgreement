/*
 *  CASRequest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "CASRequest.hpp"
#include "../../util/utils.hpp"
#define CLASS_NAME	"CASWriteRequest"

#include <string>


CASRequest::CASRequest(
		err_promise_t				&prom,
		const size_t 				memoryServerNum,
		const Pointer				&expectedPointer,
		const Pointer				&desiredPointer,
		const primitive::offset_t	offset,
		Pointer						&actualHead)
: Request(prom, memoryServerNum, Request::RequestType::CAS, Request::RegionType::HASH)
{
	params_.reset(new CASParameters(expectedPointer, desiredPointer, offset, actualHead));

	//std::string s = "CASR: ms, b = (" +  utilities::ToString<int>(memoryServerNum) + "||" + utilities::ToString<int>(offset) + ")";
	//std::cout << s << std::endl;
}

std::shared_ptr<CASRequest::CASParameters>& CASRequest::getParameters() {
	return params_;
}

CASRequest::~CASRequest() {
	// DEBUG_COUT(CLASS_NAME, __func__, "Destroyed!");
}
