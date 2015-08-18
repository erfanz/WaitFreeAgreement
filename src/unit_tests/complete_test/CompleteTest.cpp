/*
 *  CompleteTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 17, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "CompleteTest.hpp"
#include "../agents_test/AgentsHandler.hpp"
#include "../../agents/memory_server/MemoryRequestDispatcher.hpp"
#include "../../errors/Error.hpp"
#include <assert.h>
#include <stdlib.h>		// srand, rand
#include <time.h>		// time
#include <thread>		// std::thread

#define CLASS_NAME	"CompleteTest"

typedef error::ErrorType ErrorType;

std::vector<std::function<void()>> CompleteTest::functionList {
	test_all
};

std::vector<std::function<void()>>& CompleteTest::getFunctionList() {
	return functionList;
}

void CompleteTest::test_all() {
	TestBase::printMessage(CLASS_NAME, __func__);
	ErrorType eType;
	agents_handler::setup();
	srand ((unsigned int)time(NULL));

	std::thread dispatcherThread (&MemoryRequestDispatcher::run, std::ref(*agents_handler::dispatcher));     // spawn new thread that calls foo()

	Key k1("k1");
	Value v1;
	eType = agents_handler::atomicUpdates.at(0)->get(k1, v1);
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "get Value[" << k1.getId() << "] = " << v1.getContent());
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in Value[" << k1.getId() << "]");

	v1.setContent("v1");
	eType = agents_handler::atomicUpdates.at(0)->set(k1, v1);
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "set Value[" << k1.getId() << "] = " << v1.getContent());
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in Value[" << k1.getId() << "]");

	eType = agents_handler::atomicUpdates.at(0)->serialize();
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "serialized successfully");
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in serialization");
}
