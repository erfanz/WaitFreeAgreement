/*
 *  Status.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 7, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "EntryState.hpp"

std::map<int,std::string> EntryState::getStateName = {
   {EntryState::State::UNKNOWN,		"UNKNOWN"},
   {EntryState::State::SERIALIZED,	"SERIALIZED"},
   {EntryState::State::FAILED,		"FAILED"}
};
