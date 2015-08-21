/*
 *  utils.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef UTIL_UTILS_HPP_
#define UTIL_UTILS_HPP_

#include <stdint.h>
#include <byteswap.h>
#include <string>
#include <bitset>
#include <sstream>	// for stringstream
#include <iomanip>	// std::setw()
#include "../config.hpp"


namespace utilities {
	#if(DEBUG_ENABLED)
	#define DEBUG_COUT(className,funcName,message) do { \
			std::string header = std::string("[") + className + "::" + funcName + "] "; \
			std::cout << std::setw(35) << std::left << header << message << std::endl; \
		} while( false )
	#define DEBUG_CERR(className,funcName,message) do { \
			std::string header = std::string("[") + className + "::" + funcName + "] "; \
			std::cerr << std::setw(35) << std::left << header << message << std::endl; \
		} while( false )
	#else
	#define DEBUG_COUT(className,funcName,message) do {} while (false)
	#define DEBUG_CERR(className,funcName,message) do {} while (false)
	#endif


	#define TEST_NZ(x) do { if ( (x)) die("error: " #x " failed (returned non-zero).");  } while (0)
	#define TEST_Z(x)  do { if (!(x)) die("error: " #x " failed (returned zero/null)."); } while (0)

	#if defined(__i386__)
	static __inline__ unsigned long long rdtsc(void)
	{
		unsigned long long int x;
		__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
		return x;
	}
	#elif defined(__x86_64__)
	static __inline__ unsigned long long rdtsc(void)
	{
		unsigned hi, lo;
		__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
		return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
	}
	#endif




//	#if __BYTE_ORDER == __LITTLE_ENDIAN
//
//	static inline uint64_t hton64 (uint64_t x){
//		bswap_64 (x);
//	}
//	static inline uint32_t hton32 (uint32_t x){
//		bswap_32 (x);
//	}
//
//	#elif __BYTE_ORDER == __BIG_ENDIAN
//	static inline uint64_t hton64 (uint64_t x)
//	{
//		return x;
//	}
//	static inline uint32_t hton32 (uint32_t x)
//	{
//		return x;
//	}
//
//	#else
//	#error __BYTE_ORDER is neither __LITTLE_ENDIAN nor __BIG_ENDIAN
//	#endif


	/******************************************************************************
	 * Function: generate_random_seed
	 *
	 * Input
	 * None
	 *
	 * Returns
	 * a random number, which can be used an input to srand()
	 *
	 * Description
	 * Generates a random seed to be used for srand() function
	 ******************************************************************************/
	unsigned long generate_random_seed();

	uint64_t binary_string_to_ull(std::string s);


	// the implementation of templated functions cannot be hidden inside the cpp file, in most compilers.
	template <typename T>
	std::string ToString(T val) {
	    std::stringstream stream;
	    stream << val;
	    return stream.str();
	}

	int numDigits(int x);

	void die(const char *reason);
}

#endif /* UTIL_UTILS_HPP_ */
