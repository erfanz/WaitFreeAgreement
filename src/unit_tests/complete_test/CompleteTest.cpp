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
#include <vector>
#include <algorithm>	// std::random_shuffle

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
	Value v;
	agents_handler::setup();
	srand ((unsigned int)time(NULL));

	std::vector<Key> allKeys;

	for (size_t i = 0; i < config::KEY_CNT; i++)
		allKeys.push_back(Key ("k" + utilities::ToString<size_t>(i)));

	std::thread dispatcherThread (&MemoryRequestDispatcher::run, std::ref(*agents_handler::dispatcher));     // spawn new thread that calls foo()

	for (size_t i = 0; i < 50; i++) {
		primitive::coordinator_num_t cID = (primitive::coordinator_num_t) rand() % config::COORDINATOR_CNT;

		size_t updatesCnt = (rand() % 1) + 1;
		size_t depCnt = (rand() % 2) + 1;

		std::vector<Key> keys = allKeys;
		std::random_shuffle(keys.begin(), keys.end());

		std::vector<Key>::const_iterator first = keys.begin();
		std::vector<Key>::const_iterator last = keys.begin() + updatesCnt;
		std::vector<Key> updatesKeys(first, last);

		std::vector<Key>::const_iterator first2 = keys.begin() + updatesCnt;
		std::vector<Key>::const_iterator last2 = keys.begin() + updatesCnt + depCnt;
		std::vector<Key> dependencyKeys(first2, last2);

		dependencyKeys.insert(dependencyKeys.end(), updatesKeys.begin(), updatesKeys.end());

		std::string concatValue = "";

		std::cout << "Starting Atomic Change: "  << "get (";
		for (size_t i = 0; i < dependencyKeys.size(); i++)
			std::cout << dependencyKeys.at(i).getId() << ", ";
		std::cout << ")   set (";
		for (size_t i = 0; i < updatesKeys.size(); i++)
			std::cout << updatesKeys.at(i).getId() << ", ";
		std::cout << ")" << std::endl;

		// Reset the AtomicUpcate object
		agents_handler::atomicUpdates.at(cID)->reset();

		// Get the dependencies
		for (size_t i = 0; i < dependencyKeys.size(); i++){
			std::cout << "-----------------------------------------------------------" << std::endl;
			const Key &k = dependencyKeys.at(i);
			eType = agents_handler::atomicUpdates.at(cID)->get(k, v);

			if (eType == error::SUCCESS) {
				concatValue += v.getContent() + "";
				DEBUG_COUT(CLASS_NAME, __func__, "get Value[" << k.getId() << "] = " << v.getContent());
			}
			else if (eType == error::ENTRY_DOES_NOT_EXIST)
				DEBUG_COUT(CLASS_NAME, __func__, "No value for key " << k.getId());
			else
				DEBUG_CERR(CLASS_NAME, __func__, "Error in Value[" << k.getId() << "]");
		}

		// Set the updates
		for (size_t i = 0; i < updatesKeys.size(); i++){
			std::cout << "-----------------------------------------------------------" << std::endl;
			const Key &k = updatesKeys.at(i);
			eType = agents_handler::atomicUpdates.at(cID)->set(k, agents_handler::keyToValueMap[k.getId()] + "" + concatValue);
			if (eType == error::SUCCESS)
				DEBUG_COUT(CLASS_NAME, __func__, "set Value[" << k.getId() << "] = " << concatValue);
			else
				DEBUG_CERR(CLASS_NAME, __func__, "Error in Value[" << k.getId() << "]. Error Code: " << eType);
		}

		std::cout << "-----------------------------------------------------------" << std::endl;
		// Serialize
		eType = agents_handler::atomicUpdates.at(cID)->serialize();
		if (eType == error::SUCCESS)
			DEBUG_COUT(CLASS_NAME, __func__, "serialized successfully");
		else
			DEBUG_CERR(CLASS_NAME, __func__, "Error in serialization");
	}
}
