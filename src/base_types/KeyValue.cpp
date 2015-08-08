/*
 *  KeyValue.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */


#include "KeyValue.hpp"

KeyValue::KeyValue(){
}


KeyValue::KeyValue(const Key &k, const Value &v) {
	this->key_ = k;
	this->value_ = v;
}


const Key& KeyValue::getKey() const{
	return key_;
}

const Value& KeyValue::getValue() const {
	return value_;
}

size_t KeyValue::getTotalSize() const {
	size_t size = 0;
	size += key_.getId().size();
	size += 1;		// one space between the key and value
	size += value_.getContent().size();
	return size;
}

std::string KeyValue::toString() const{
	std::ostringstream os;
	serialize(os);
	return os.str();
}

bool KeyValue::isEqual(const KeyValue &kv) const {
	if (key_.isEqual(kv.getKey())
			&& value_.isEqual(kv.getValue()))
		return true;
	else return false;
}

void KeyValue::serialize(std::ostream& stream) const{
	stream << key_.getId() << " "
			<< value_.getContent();
}

void KeyValue::doDeserialize(std::istream& stream, KeyValue &kv){
	std::string id;
	std::string content;

	stream >> id >> content;

	kv.key_.setId(id);
	kv.value_.setContent(content);
}
