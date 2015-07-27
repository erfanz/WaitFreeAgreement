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
#include "../errors/Error.hpp"
#include <string>
#include <vector>

typedef error::ErrorType ErrorType;

class AtomicUpdate {
private:
	Coordinator* coordinator_;
	std::vector<Dependency> dependencies_;
	std::vector<KeyValue> updates_;

public:
	AtomicUpdate(Coordinator *coordinator);
	ErrorType	addDependency(const Key &key, Value &value);
	ErrorType	addUpdate(const Key &key, const Value &value);
	ErrorType	apply();
	~AtomicUpdate();
};

#endif /* ATOMICUPDATE_HPP_ */