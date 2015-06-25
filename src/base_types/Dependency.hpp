/*
 *  Dependency.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 24, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef DEPENDENCY_HPP_
#define DEPENDENCY_HPP_

#include "Pointer.hpp"
#include "Serializable.hpp"
#include <string>


class Dependency : public Serializable <Dependency> {
private:
	int bucketID;
	Pointer pointer;

public:
	Dependency();
	Dependency(int bucketID, Pointer pointer);
	int getBucketID() const;
	Pointer getPointer() const;
	void setBucketID(int bucketID);
	void setPointer(Pointer p);
	bool isEqual(const Dependency &) const;
	std::string toString() const;
	virtual void serialize(std::ostream& stream) const;
	static void doDeserialize(std::istream&, Dependency &);
};

#endif /* DEPENDENCY_HPP_ */
