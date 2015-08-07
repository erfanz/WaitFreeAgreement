/*
 *  Error.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 23, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef ERROR_HPP_
#define ERROR_HPP_

// #include "Throwable.hpp"

#define TEST_NZ(x) do { if ( (x)) die("error: " #x " failed (returned non-zero).");  } while (0)
#define TEST_Z(x)  do { if (!(x)) die("error: " #x " failed (returned zero/null)."); } while (0)

// #define TEST_ERROR(x, handleError)  do {result if (!(x)) handleError("error: " #x " failed (returned zero/null)."); } while (0)


namespace error{
	enum ErrorType {
		SUCCESS = 0,
		CAS_FAILED,
		KEY_NOT_FOUND,
		NO_UPDATE_KEY_IN_CHANGE,
		DIFFERENT_BUCKET_HEADS,
		NOT_MARKED_SERIALIZED,
		CHANGE_FAILURE,
		UNSERIALIZABLE,
		RESOLVE_FAILED,
		SET_WITH_NO_GET,
		GET_POINTER_CHANGED
	};

	class Throwable;
}

class error::Throwable {
public:
	// virtual void errorHandler(const ErrorType eType);
	// void testError(const ErrorType eType);

    virtual ~Throwable(){}
};

#endif /* ERROR_HPP_ */
