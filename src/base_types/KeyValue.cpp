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


KeyValue::KeyValue(Key k, Value v, Pointer n) {
	this->key = k;
	this->value = v;
	this->nextPointer = n;
}


Key KeyValue::getKey() const{
	return key;
}
Value KeyValue::getValue() const {
	return value;
}
Pointer KeyValue::getNextPointer() const{
	return nextPointer;
}

std::string KeyValue::toString() const{
	std::ostringstream os;
	serialize(os);
	return os.str();
}

bool KeyValue::isEqual(const KeyValue &kv) const {
	if (key.isEqual(kv.getKey())
			&& value.isEqual(kv.getValue())
			&& nextPointer.isEqual(kv.getNextPointer()))
		return true;
	else return false;
}

void KeyValue::serialize(std::ostream& stream) const{
	stream << key.getId() << " "
			<< value.getContent() << " "
			<< nextPointer.toString();
}

void KeyValue::doDeserialize(std::istream& stream, KeyValue &kv){
	std::string id;
	std::string content;
	std::string p;

	stream >> id >> content >> p;

	kv.key.setId(id);
	kv.value.setContent(content);
	Pointer::fromString(p, kv.nextPointer);
}
