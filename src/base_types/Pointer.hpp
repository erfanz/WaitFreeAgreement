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

typedef uint8_t coordinator_num_t;
typedef uint8_t generation_num_t;
typedef uint16_t entry_size_t;
typedef uint32_t offset_t;
typedef uint64_t pointer_size_t;



class Pointer : public Serializable<Pointer>{
private:
	coordinator_num_t coordinatorNum;
	generation_num_t generationNum;
	entry_size_t length;
	offset_t offset;


public:
	Pointer();
	Pointer(coordinator_num_t coordinatorNum, generation_num_t generationNum, entry_size_t length, offset_t offset);
	virtual void serialize(std::ostream& stream) const;	// inherited from Serializable
	static Pointer makePointer(pointer_size_t);

	const coordinator_num_t getCoordinatorNum() const;
	const generation_num_t getGenerationNum() const;
	const entry_size_t getLength() const;
	const offset_t getOffset() const;
	static size_t getTotalSize();

	void setCoordinatorNum(coordinator_num_t coordinatorNum);
	void setGenerationNum(generation_num_t generationNum);
	void setLength(entry_size_t length);
	void setOffset(offset_t offset);
	void setAll(coordinator_num_t coordinatorNum, generation_num_t generationNum, entry_size_t length, offset_t offset);


	std::string toString() const;
	bool isEqual(const Pointer &pointer) const;
	static void fromString(std::string serialized, Pointer &pointer);
	static void doDeserialize(std::istream& stream, Pointer &p);
};

#endif /* POINTER_HPP_ */
