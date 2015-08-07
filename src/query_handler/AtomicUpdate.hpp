/*
 *  AtomicUpdate.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 9, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef ATOMICUPDATE_HPP_
#define ATOMICUPDATE_HPP_

#include "../agents/coordinator/Coordinator.hpp"
#include "../base_types/Dependency.hpp"
#include "../base_types/KeyValue.hpp"
#include "../base_types/Pointer.hpp"
#include "../errors/Error.hpp"
#include <string>
#include <vector>
#include <map>

typedef error::ErrorType ErrorType;

class AtomicUpdate {
private:
	Coordinator* coordinator_;
	std::map<size_t, Pointer> dependencyMap_;
	std::vector<KeyValue> updates_;
	LogEntry *newEntry;


public:
	AtomicUpdate(Coordinator *coordinator);
	ErrorType	get(const Key &key, Value &value);
	ErrorType	set(const Key &key, const Value &value);
	ErrorType	serialize();
	~AtomicUpdate();
};

#endif /* ATOMICUPDATE_HPP_ */
