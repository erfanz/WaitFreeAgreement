/*
 *  Error.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 30, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Error.hpp"

void error::Throwable::testError(const ErrorType eType) {
	if (eType != SUCCESS)
		errorHandler(eType);
}
