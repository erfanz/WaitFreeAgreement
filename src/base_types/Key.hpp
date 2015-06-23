/*
 *  Key.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef BASE_CALSSES_KEY_HPP_
#define BASE_CALSSES_KEY_HPP_

#include "HashMaker.hpp"
#include <stdint.h>
#include <string>

class Key{
private:
	std::string id;

public:
	Key();
	Key(std::string id);
	std::string getId() const;
	void setId(std::string id);
	HashMaker hashKey() const;
	bool isEqual(const Key &key) const;
};

#endif /* BASE_CALSSES_KEY_HPP_ */
