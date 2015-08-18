/*
 *  LogJournal.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 17, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "LogJournal.hpp"
#include "../../util/utils.hpp"

#define CLASS_NAME	"LogJournal"

LogJournal::LogJournal() {
	content_ = new std::atomic<char>[config::LOG_JOURNAL_SIZE];
	region_ = new LocalRegionContext<char>(content_);
}

void LogJournal::resetRegion() {
	for (size_t i = 0; i < config::LOG_JOURNAL_SIZE; i++)
		content_[i] = 0;
}

LogJournal::~LogJournal() {
	delete[] content_;
	delete region_;
	DEBUG_COUT (CLASS_NAME, __func__, "LogJournal destroyed!");
}
