/*
 *  utils.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */


#include "utils.hpp"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

// http://www.concentric.net/~Ttwang/tech/inthash.htm
unsigned long utilities::generate_random_seed()
{
	unsigned long a = clock();
	unsigned long b = time(NULL);
	unsigned long c = getpid();

    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);

	return c;
}

void utilities::die(const char *reason)
{
	std::cerr << reason << std::endl;
	std::cerr << "Errno: " << strerror(errno) << std::endl;
	exit(EXIT_FAILURE);
}

uint64_t utilities::binary_string_to_ull(std::string s){
	// return std::stoull (p1, nullptr, 16);	// 16 specifies the base, so it's hex

	std::bitset<64> bit_rep(s);
	return bit_rep.to_ullong();
}

int utilities::numDigits(int x) {
	x = abs(x);
	return (x < 10 ? 1 :
		(x < 100 ? 2 :
		(x < 1000 ? 3 :
		(x < 10000 ? 4 :
		(x < 100000 ? 5 :
		(x < 1000000 ? 6 :
		(x < 10000000 ? 7 :
		(x < 100000000 ? 8 :
		(x < 1000000000 ? 9 :
		10)))))))));
}
