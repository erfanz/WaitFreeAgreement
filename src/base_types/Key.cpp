/*
 *  Key.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Key.hpp"
#include "../config.hpp"

Key::Key() {
	;
}
Key::Key(std::string id) {
	this->id = id;
}

std::string Key::getId() const {
	return id;
}

void Key::setId(std::string id) {
	this->id = id;
}

HashMaker Key::hashKey() const {
	return HashMaker(id);
}

bool Key::isEqual(const Key &key) const {
	return (key.id.compare(this->id) == 0) ? true : false;
}
