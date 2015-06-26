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

Pointer::Pointer(coordinator_num_t coordinatorNum, generation_num_t generationNum, entry_size_t length, offset_t offset){
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

	p.coordinatorNum = (coordinator_num_t) (concat >> 56);
	p.generationNum = (generation_num_t)( (concat << 8) >> 56 );
	p.length = (entry_size_t)( (concat << 16) >> 48 );
	p.offset = (offset_t)( (concat << 32) >> 32 );
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

Pointer Pointer::makePointer(pointer_size_t input){
	coordinator_num_t coordinatorNum = (coordinator_num_t) (input >> 56);
	generation_num_t generationNum = (generation_num_t)( (input << 8) >> 56 );
	entry_size_t length = (entry_size_t)( (input << 16) >> 48 );
	offset_t offset = (offset_t)( (input << 32) >> 32 );

	Pointer p(coordinatorNum, generationNum, length, offset);
	return p;
}

const coordinator_num_t Pointer::getCoordinatorNum() const{
	return coordinatorNum;
}

const generation_num_t Pointer::getGenerationNum() const{
	return generationNum;
}

const entry_size_t Pointer::getLength() const{
	return length;
}

const offset_t Pointer::getOffset() const{
	return offset;
}

size_t Pointer::getTotalSize(){
	return 64;
}

void Pointer::setCoordinatorNum(coordinator_num_t coordinatorNum) {
	this->coordinatorNum = coordinatorNum;
}

void Pointer::setGenerationNum(generation_num_t generationNum) {
	this->generationNum = generationNum;
}

void Pointer::setLength(entry_size_t length) {
	this->length = length;
}

void Pointer::setOffset(offset_t offset) {
	this->offset = offset;
}

void Pointer::setAll(coordinator_num_t coordinatorNum, generation_num_t generationNum, entry_size_t length, offset_t offset) {
	this->coordinatorNum = coordinatorNum;
	this->generationNum = generationNum;
	this->length = length;
	this->offset = offset;
}

bool Pointer::isEqual(const Pointer &pointer) const {
	if (this->coordinatorNum == pointer.coordinatorNum
			&& this->generationNum == pointer.generationNum
			&& this->length == pointer.length
			&& this->offset == pointer.offset)
		return true;
	else return false;
}
