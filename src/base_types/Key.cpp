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
	this->id_ = id;
}

const std::string& Key::getId() const {
	return id_;
}

void Key::setId(const std::string& id) {
	this->id_ = id;
}

HashMaker Key::hashKey() const {
	return HashMaker(id_);
}

bool Key::isEqual(const Key &key) const {
	return (key.id_.compare(this->id_) == 0) ? true : false;
}
