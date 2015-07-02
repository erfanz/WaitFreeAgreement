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
	size_t bucketID_;
	Pointer pointer_;

public:
	Dependency();
	Dependency(size_t bucketID, Pointer pointer);
	const size_t getBucketID() const;
	const Pointer getPointer() const;
	void setBucketID(const size_t bucketID);
	void setPointer(const Pointer p);
	bool isEqual(const Dependency &) const;
	std::string toString() const;
	virtual void serialize(std::ostream& stream) const;
	static void doDeserialize(std::istream&, Dependency &);
};

#endif /* DEPENDENCY_HPP_ */
