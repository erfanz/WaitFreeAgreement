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

Pointer::Pointer() {
	;
}

Pointer::Pointer(primitive::coordinator_num_t coordinatorNum, primitive::generation_num_t generationNum, primitive::entry_size_t length, primitive::offset_t offset){
	this->coordinatorNum_ = coordinatorNum;
	this->generationNum_ = generationNum;
	this->length_ = length;
	this->offset_ = offset;
}

void Pointer::serialize(std::ostream& stream) const{
	primitive::pointer_size_t concat = toULL();

	// stream << std::hex << std::setfill('0') << std::setw(16) << concat;

	std::bitset<64> binary_rep(concat);
	stream << binary_rep;
}

void Pointer::doDeserialize(std::istream& stream, Pointer &p){
	uint64_t concat;
	std::string binary_str;
	stream >> binary_str;

	concat = utilities::binary_string_to_ull(binary_str);

	p.coordinatorNum_ = (primitive::coordinator_num_t) (concat >> 56);
	p.generationNum_ = (primitive::generation_num_t)( (concat << 8) >> 56 );
	p.length_ = (primitive::entry_size_t)( (concat << 16) >> 48 );
	p.offset_ = (primitive::offset_t)( (concat << 32) >> 32 );
}

primitive::pointer_size_t Pointer::toULL() const{
	primitive::pointer_size_t concat = 0ULL;

	// concatenating the 'coordinatorNum' and shifting
	concat |= coordinatorNum_;
	concat <<= 8;

	// concatenating the 'generationNum' and shifting
	concat |= generationNum_;
	concat <<= 16;

	// concatenating the 'length' and shifting
	concat |= length_;
	concat <<= 32;

	// concatenating the 'offset'. there is no need for shifting
	concat |= offset_;

	return concat;
}

std::string Pointer::toString() const{
	std::ostringstream os;
	serialize(os);
	return os.str();
}

std::string Pointer::toHexString() const{
	std::string output = "";
	output += std::string(utilities::ToString<int>((int)coordinatorNum_)) + "|"
			+ utilities::ToString<int>((int)generationNum_) + "|"
			+ utilities::ToString<primitive::entry_size_t>(length_) + "|"
			+ utilities::ToString<primitive::offset_t>(offset_);

	return output;
}

void Pointer::fromBinaryString(std::string serialized, Pointer &p){
	std::istringstream is(serialized);
	doDeserialize(is, p);
}

Pointer Pointer::makePointer(const primitive::pointer_size_t input){
	primitive::coordinator_num_t coordinatorNum = (primitive::coordinator_num_t) (input >> 56);
	primitive::generation_num_t generationNum = (primitive::generation_num_t)( (input << 8) >> 56 );
	primitive::entry_size_t length = (primitive::entry_size_t)( (input << 16) >> 48 );
	primitive::offset_t offset = (primitive::offset_t)( (input << 32) >> 32 );

	Pointer p(coordinatorNum, generationNum, length, offset);
	return p;
}

const primitive::coordinator_num_t Pointer::getCoordinatorNum() const{
	return coordinatorNum_;
}

const primitive::generation_num_t Pointer::getGenerationNum() const{
	return generationNum_;
}

const primitive::entry_size_t Pointer::getLength() const{
	return length_;
}

const primitive::offset_t Pointer::getOffset() const{
	return offset_;
}

size_t Pointer::getTotalSize(){
	// return sizeof(uint64_t);
	return 64;	// TODO: since we're writing each pointer as a 64-character boolean sequence
}

const bool Pointer::isNull() const {
	if (coordinatorNum_ == 0
			&& generationNum_ == 0
			&& length_ == 0
			&& offset_ == 0)
		return true;
	else return false;
}

void Pointer::setCoordinatorNum(const primitive::coordinator_num_t coordinatorNum) {
	this->coordinatorNum_ = coordinatorNum;
}

void Pointer::setGenerationNum(const primitive::generation_num_t generationNum) {
	this->generationNum_ = generationNum;
}

void Pointer::setLength(const primitive::entry_size_t length) {
	this->length_ = length;
}

void Pointer::setOffset(const primitive::offset_t offset) {
	this->offset_ = offset;
}


bool Pointer::isEqual(const Pointer &pointer) const {
	if (this->coordinatorNum_ == pointer.coordinatorNum_
			&& this->generationNum_ == pointer.generationNum_
			&& this->length_ == pointer.length_
			&& this->offset_ == pointer.offset_)
		return true;
	else return false;
}
