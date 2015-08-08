/*
 *  Status.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 7, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef STATUS_HPP_
#define STATUS_HPP_

#include <map>
#include <string>
class EntryState{
public:
	enum State {
		UNKNOWN = 0,
		SERIALIZED = 1,
		FAILED = 2
	};

    static std::map<int, std::string> getStateName;
};

#endif /* STATUS_HPP_ */
