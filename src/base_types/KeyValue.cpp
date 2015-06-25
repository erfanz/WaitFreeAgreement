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


KeyValue::KeyValue(Key k, Value v) {
	this->key = k;
	this->value = v;
}


Key KeyValue::getKey() const{
	return key;
}
Value KeyValue::getValue() const {
	return value;
}


std::string KeyValue::toString() const{
	std::ostringstream os;
	serialize(os);
	return os.str();
}

bool KeyValue::isEqual(const KeyValue &kv) const {
	if (key.isEqual(kv.getKey())
			&& value.isEqual(kv.getValue()))
		return true;
	else return false;
}

void KeyValue::serialize(std::ostream& stream) const{
	stream << key.getId() << " "
			<< value.getContent();
}

void KeyValue::doDeserialize(std::istream& stream, KeyValue &kv){
	std::string id;
	std::string content;

	stream >> id >> content;

	kv.key.setId(id);
	kv.value.setContent(content);
}
