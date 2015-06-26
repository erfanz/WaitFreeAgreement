/*
 *  Value.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Value.hpp"

Value::Value(){
	;
}

Value::Value(std::string content){
	this->content = content;
}

const std::string& Value::getContent() const{
	return content;
}

uint32_t Value::getSize() const{
	return size;
}

void Value::setContent(const std::string &content) {
	this->content = content;
}

bool Value::isEqual(const Value &v) const{
	if (this->content.compare(v.getContent()) == 0)
		return true;
	else return false;
}

