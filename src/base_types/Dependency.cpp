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

Dependency::Dependency() {
	;
}

Dependency::Dependency(int bucketID, Pointer pointer) {
	this->bucketID = bucketID;
	this->pointer = pointer;
}

int Dependency::getBucketID() const {
	return bucketID;
}
Pointer Dependency::getPointer() const {
	return pointer;
}

void Dependency::setBucketID(int bucketID) {
	this->bucketID = bucketID;
}

void Dependency::setPointer(Pointer p) {
	this->pointer = p;
}

bool Dependency::isEqual(const Dependency &dependency) const{
	if (bucketID == dependency.bucketID && pointer.isEqual(dependency.pointer))
			return true;
	return false;
}

std::string Dependency::toString() const{
	std::ostringstream os;
	serialize(os);
	return os.str();
}

void Dependency::serialize(std::ostream& stream) const{
	std::string str = utilities::ToString<int>(bucketID) + " " + pointer.toString();
	stream << str;
}
void Dependency::doDeserialize(std::istream& stream, Dependency &d){
	std::string p;
	stream >> d.bucketID >> p;
	Pointer::fromString(p, d.pointer);
}
