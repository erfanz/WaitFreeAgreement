/*
 *  Change.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 25, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef CHANGE_HPP_
#define CHANGE_HPP_

#include "Dependency.hpp"
#include "KeyValue.hpp"
#include <vector>

class Change {
private:
	std::vector<KeyValue> updates;
	std::vector<Dependency> dependencies;
public:
	Change(std::vector<KeyValue> updates, std::vector<Dependency> dependencies);
	~Change();
	const std::vector<Dependency>& getDependencies() const;
	const std::vector<KeyValue>& getUpdates() const;
};

#endif /* CHANGE_HPP_ */
