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

	agents_handler::atomicUpdates.at(1)->reset();
	Key k1("k1");
	Value v1;
	eType = agents_handler::atomicUpdates.at(1)->get(k1, v1);
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "get Value[" << k1.getId() << "] = " << v1.getContent());
	else if (eType == error::ENTRY_DOES_NOT_EXIST)
			DEBUG_COUT(CLASS_NAME, __func__, "No value for key " << k1.getId());
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in Value[" << k1.getId() << "]");

	v1.setContent("v1");
	eType = agents_handler::atomicUpdates.at(1)->set(k1, v1);
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "set Value[" << k1.getId() << "] = " << v1.getContent());
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in Value[" << k1.getId() << "]");

	eType = agents_handler::atomicUpdates.at(1)->serialize();
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "serialized successfully");
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in serialization");

	std::cout << "**************************************" << std::endl;


	agents_handler::atomicUpdates.at(0)->reset();
	Key k2("k2");
	Value v2;
	eType = agents_handler::atomicUpdates.at(0)->get(k1, v1);
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "get Value[" << k1.getId() << "] = " << v1.getContent());
	else if (eType == error::ENTRY_DOES_NOT_EXIST)
		DEBUG_COUT(CLASS_NAME, __func__, "No value for key " << k1.getId());
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in Value[" << k1.getId() << "]");

	eType = agents_handler::atomicUpdates.at(0)->get(k2, v2);
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "get Value[" << k2.getId() << "] = " << v2.getContent());
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in Value[" << k2.getId() << "]");

	v2.setContent("v2");
	eType = agents_handler::atomicUpdates.at(0)->set(k2, v2);
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "set Value[" << k2.getId() << "] = " << v2.getContent());
	else if (eType == error::ENTRY_DOES_NOT_EXIST)
		DEBUG_COUT(CLASS_NAME, __func__, "No value for key " << k1.getId());
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in Value[" << k2.getId() << "]");

	eType = agents_handler::atomicUpdates.at(0)->serialize();
	if (eType == error::SUCCESS)
		DEBUG_COUT(CLASS_NAME, __func__, "serialized successfully");
	else
		DEBUG_CERR(CLASS_NAME, __func__, "Error in serialization");
}
