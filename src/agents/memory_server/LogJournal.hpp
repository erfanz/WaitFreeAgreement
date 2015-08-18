/*
 *  LogJournal.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef LOGJOURNAL_HPP_
#define LOGJOURNAL_HPP_

#include "../../region/LocalRegionContext.hpp"
#include "../../config.hpp"
#include <atomic>



class LogJournal {
private:
	std::atomic<char> *content_;

public:
	LocalRegionContext<char> *region_;

	LogJournal();
	void resetRegion();
	~LogJournal();
};

#endif /* LOGJOURNAL_HPP_ */
