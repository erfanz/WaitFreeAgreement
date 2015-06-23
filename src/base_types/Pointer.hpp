/*
 * Pointer.hpp
 *
 *  Created on: Jun 9, 2015
 *      Author: Erfan Zamanian
 */

#ifndef POINTER_HPP_
#define POINTER_HPP_

#include "Serializable.hpp"
#include <iostream>      // std::ios, std::istream, std::cout
#include <stdint.h>
#include <string>

class Pointer : public Serializable<Pointer>{
private:
	uint8_t coordinatorNum;
	uint8_t generationNum;
	uint16_t length;
	uint32_t offset;


public:
	Pointer();
	Pointer(uint8_t coordinatorNum, uint8_t generationNum, uint16_t length, uint32_t offset);
	virtual void serialize(std::ostream& stream) const;	// inherited from Serializable
	static Pointer makePointer(uint64_t);
	uint8_t getCoordinatorNum() const;
	uint8_t getGenerationNum() const;
	uint16_t getLength() const;
	uint32_t getOffset() const;
	std::string toString() const;
	bool isEqual(const Pointer &pointer) const;
	static void fromString(std::string serialized, Pointer &pointer);
	static void doDeserialize(std::istream& stream, Pointer &p);


};

#endif /* POINTER_HPP_ */
