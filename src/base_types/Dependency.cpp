/*
 *  Dependency.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 24, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "Dependency.hpp"
#include "../util/utils.hpp"	// for toString()
#include <sstream>      // std::ostringstream

#define CLASS_NAME	"Dependency"


Dependency::Dependency() {
	;
}

Dependency::Dependency(const size_t bucketID, const Pointer &pointer) {
	this->bucketID_ = bucketID;
	this->pointer_ = pointer;
}

const size_t Dependency::getBucketID() const {
	return bucketID_;
}

const Pointer Dependency::getPointer() const {
	return pointer_;
}

void Dependency::setBucketID(const size_t bucketID) {
	this->bucketID_ = bucketID;
}

void Dependency::setPointer(const Pointer &p) {
	this->pointer_ = p;
}

bool Dependency::isEqual(const Dependency &dependency) const{
	if (bucketID_ == dependency.bucketID_ && pointer_.isEqual(dependency.pointer_))
			return true;
	return false;
}

std::string Dependency::toString() const{
	std::ostringstream os;
	serialize(os);
	return os.str();
}

void Dependency::serialize(std::ostream& stream) const{
	std::string str = utilities::ToString<size_t>(bucketID_) + " " + pointer_.toString();
	stream << str;
}
void Dependency::doDeserialize(std::istream& stream, Dependency &d){
	std::string pointerString;
	stream >> d.bucketID_ >> pointerString;
	Pointer::fromBinaryString(pointerString, d.pointer_);
}
