/*
 *  Change.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jul 9, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef CHANGE_HPP_
#define CHANGE_HPP_

#include "../agents/coordinator/Coordinator.hpp"
#include "../base_types/Dependency.hpp"
#include "../base_types/KeyValue.hpp"
#include "../base_types/Pointer.hpp"
#include "../errors/Error.hpp"
#include <string>
#include <vector>
#include <map>

typedef error::ErrorType ErrorType;

class Change {
private:
	Coordinator* coordinator_;					// the requested coordinator to be associated with the Change.
	std::map<size_t, Pointer> dependencyMap_;	// the map from bucketIDs to their associated Pointer at the time of get()
	std::vector<KeyValue> updates_;				// the list of all key-value updates, specified by set()
	LogEntry *newEntry;							// the log entry to be constructed with the new updates and dependencies.

public:
	Change(Coordinator *coordinator);
	void reset();
	ErrorType	get(const Key &key, Value &value);		// it's getCur()
	ErrorType	set(const Key &key, const Value &value);
	ErrorType	serialize();
	~Change();
};

#endif /* CHANGE_HPP_ */
