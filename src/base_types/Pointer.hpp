/*
 * Pointer.hpp
 *
 *  Created on: Jun 9, 2015
 *      Author: Erfan Zamanian
 */

#ifndef POINTER_HPP_
#define POINTER_HPP_

#include "Serializable.hpp"
#include "PrimitiveTypes.hpp"	// primitive::XXX
#include <iostream>      // std::ios, std::istream, std::cout
#include <stdint.h>
#include <string>


class Pointer : public Serializable<Pointer>{
private:
	primitive::coordinator_num_t coordinatorNum_;
	primitive::generation_num_t generationNum_;
	primitive::entry_size_t length_;
	primitive::offset_t offset_;

public:
	Pointer();
	Pointer(const primitive::coordinator_num_t coordinatorNum, const primitive::generation_num_t generationNum, const primitive::entry_size_t length, const primitive::offset_t offset);
	bool operator< (const Pointer &right) const;		// needed for set operation
	bool operator> (const Pointer &right) const;		// needed for set operation
	bool isEqual(const Pointer &pointer) const;

	virtual void serialize(std::ostream& stream) const;	// inherited from Serializable
	static Pointer makePointer(const primitive::pointer_size_t);

	const primitive::coordinator_num_t getCoordinatorNum() const;
	const primitive::generation_num_t getGenerationNum() const;
	const primitive::entry_size_t getLength() const;
	const primitive::offset_t getOffset() const;
	static size_t getTotalSize();
	const bool isNull() const;


	void setCoordinatorNum(const primitive::coordinator_num_t coordinatorNum);
	void setGenerationNum(const primitive::generation_num_t generationNum);
	void setLength(const primitive::entry_size_t length);
	void setOffset(const primitive::offset_t offset);


	primitive::pointer_size_t toULL() const;
	std::string toString() const;
	std::string toHexString() const;
	// int compare(const Pointer &pointer) const;
	static void fromBinaryString(const std::string &serialized, Pointer &pointer);
	static void doDeserialize(std::istream& stream, Pointer &p);

	void setToNull();
};

#endif /* POINTER_HPP_ */
