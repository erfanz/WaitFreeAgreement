/*
 *  Pointer.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 17, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Pointer.hpp"
#include "../util/utils.hpp"	// for binary_to_string_uul()
#include <iomanip>      // std::setw
#include <sstream>      // std::ostringstream
#include <bitset>        // std::bitset


Pointer::Pointer(){
	;
}

Pointer::Pointer(uint8_t coordinatorNum, uint8_t generationNum, uint16_t length, uint32_t offset){
	this->coordinatorNum = coordinatorNum;
	this->generationNum = generationNum;
	this->length = length;
	this->offset = offset;
}

void Pointer::serialize(std::ostream& stream) const{
	uint64_t concat = 0ULL;

	// concatenating the 'coordinatorNum' and shifting
	concat |= coordinatorNum;
	concat <<= 8;

	// concatenating the 'generationNum' and shifting
	concat |= generationNum;
	concat <<= 16;

	// concatenating the 'length' and shifting
	concat |= length;
	concat <<= 32;

	// concatenating the 'offset'. there is no need for shifting
	concat |= offset;

	// stream << std::hex << std::setfill('0') << std::setw(16) << concat;

	std::bitset<64> binary_rep(concat);
	stream << binary_rep;
}

void Pointer::doDeserialize(std::istream& stream, Pointer &p){
	uint64_t concat;
	//stream >> std::hex >> concat;
	std::string binary_str;
	stream >> binary_str;

	concat = utilities::binary_string_to_ull(binary_str);

	p.coordinatorNum = (uint8_t) (concat >> 56);
	p.generationNum = (uint8_t)( (concat << 8) >> 56 );
	p.length = (uint16_t)( (concat << 16) >> 48 );
	p.offset = (uint32_t)( (concat << 32) >> 32 );
}

std::string Pointer::toString() const{
	std::ostringstream os;
	serialize(os);
	return os.str();
}

void Pointer::fromString(std::string serialized, Pointer &p){
	std::istringstream is(serialized);
	doDeserialize(is, p);
}


Pointer Pointer::makePointer(uint64_t input){
	uint8_t coordinatorNum = (uint8_t) (input >> 56);
	uint8_t generationNum = (uint8_t)( (input << 8) >> 56 );
	uint16_t length = (uint16_t)( (input << 16) >> 48 );
	uint32_t offset = (uint32_t)( (input << 32) >> 32 );

	Pointer p(coordinatorNum, generationNum, length, offset);
	return p;
}

uint8_t Pointer::getCoordinatorNum() const{
	return coordinatorNum;
}

uint8_t Pointer::getGenerationNum() const{
	return generationNum;
}

uint16_t Pointer::getLength() const{
	return length;
}

uint32_t Pointer::getOffset() const{
	return offset;
}

bool Pointer::isEqual(const Pointer &pointer) const {
	if (this->coordinatorNum == pointer.coordinatorNum
			&& this->generationNum == pointer.generationNum
			&& this->length == pointer.length
			&& this->offset == pointer.offset)
		return true;
	else return false;
}
