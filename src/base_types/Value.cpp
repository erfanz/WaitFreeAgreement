/*
 *  Value.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Value.hpp"

#define CLASS_NAME	"Value"


Value::Value(){
	;
}

Value::Value(const std::string &content){
	this->content_ = content;
}

const std::string& Value::getContent() const{
	return content_;
}

uint32_t Value::getSize() const{
	return size_;
}

void Value::setContent(const std::string &content) {
	this->content_ = content;
}

bool Value::isEqual(const Value &v) const{
	if (this->content_.compare(v.getContent()) == 0)
		return true;
	else return false;
}

