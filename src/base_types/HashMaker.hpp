/*
 *  HashedKey.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 12, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef BASE_TYPES_HASHEDKEY_HPP_
#define BASE_TYPES_HASHEDKEY_HPP_

#include <stdint.h>
#include <functional>
#include <string>

class HashMaker {
private:
	size_t hashedKey_;

public:
	HashMaker(std::string id) {hashedKey_ =  std::hash<std::string>()(id);}
	size_t getHashed() const {return hashedKey_;}

};


#endif /* BASE_TYPES_HASHEDKEY_HPP_ */
