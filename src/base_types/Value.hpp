/*
 * Value.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 9, 2015
 *  Description	: Hello World in C++, Ansi-style
 */

#ifndef VALUE_HPP_
#define VALUE_HPP_

#include <string>
#include <stdint.h>

class Value {
private:
	uint32_t size;
	std::string content;

public:
	Value();
	Value(std::string content);
	const std::string& getContent() const;
	uint32_t getSize() const;
	void setContent(const std::string &);
	bool isEqual(const Value &v) const;
};

#endif /* VALUE_HPP_ */
