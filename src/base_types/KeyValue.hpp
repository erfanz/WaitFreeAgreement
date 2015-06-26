/*
 *  KeyValue.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 18, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef KEYVALUE_HPP_
#define KEYVALUE_HPP_

#include "Key.hpp"
#include "Value.hpp"
#include "Pointer.hpp"
#include "Serializable.hpp"

#include <string>
#include <stdint.h>
#include <sstream>      // std::ostringstream
#include <iostream>     // std::ios, std::istream, std::cout

class KeyValue : public Serializable<KeyValue>{
private:
	Key key;
	Value value;

public:
	KeyValue();
	KeyValue(Key, Value);
	const Key& getKey() const;
	const Value& getValue() const;
	size_t getTotalSize() const;
	std::string toString() const;
	bool isEqual(const KeyValue &kv) const;
	virtual void serialize(std::ostream& stream) const;
	static void doDeserialize(std::istream& stream, KeyValue &kv);

};

#endif /* KEYVALUE_HPP_ */
