/*
 *  LocalRegionContextTest.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 26, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "LocalRegionContextTest.hpp"
#include "../base_types_test/LogEntryTest.hpp"	// for makeLogEntry()
#include "../../base_types/LogEntry.hpp"	// for makeLogEntry()
#include "../../errors/Error.hpp"
#include "../../base_types/PrimitiveTypes.hpp"
#include <future>         // std::promise, std::future

#include <assert.h>
#include <string>
#include <vector>
#include <atomic>

#define CLASS_NAME	"LocalRegionContextTest"

std::vector<std::function<void()>> LocalRegionContextTest::functionList {
	test_read_and_write,
	test_read_and_write_with_logentry,
	test_CAS
};

std::vector<std::function<void()>>& LocalRegionContextTest::getFunctionList() {
	//return PointerTest::functionList;
	return functionList;
}

void LocalRegionContextTest::test_read_and_write() {
	TestBase::printMessage(CLASS_NAME, __func__);
	std::atomic<char> buffer[64];
	LocalRegionContext<char> region(buffer);

	std::string input = "this is just a test";

	region.write(input.c_str(), 0, input.size());

	char readBuffer[64];

	region.read(readBuffer, 0, input.size());


	std::string output(readBuffer);
	assert(input.compare(output) == 0);
}

void LocalRegionContextTest::test_read_and_write_with_logentry() {
	TestBase::printMessage(CLASS_NAME, __func__);

	// Create the region
	std::atomic<char> buffer[1024];
	LocalRegionContext<char> region(buffer);

	// Create the log entry
	LogEntry *entry = LogEntryTest::makeLogEntry();
	Pointer p = entry->getCurrentP();

	// Serialize the log entry and write to the region
	std::ostringstream os;
	entry->serialize(os);
	const std::string& tmp = os.str();
	const char* cstr = tmp.c_str();
	region.write(cstr, entry->getCurrentP().getOffset(), entry->getCurrentP().getLength());


	// Now we read from the region
	char readBuffer[p.getLength()];
	region.read(readBuffer, p.getOffset(), p.getLength());
	std::string tempStr(readBuffer);
	std::istringstream is(tempStr);
	LogEntry result;
	LogEntry::deserialize(is, result);

	assert(entry->isEqual(result) == true);
}

void LocalRegionContextTest::test_CAS() {
	TestBase::printMessage(CLASS_NAME, __func__);

	// Create the region
	std::atomic<uint64_t> buffer[10];
	for (size_t i = 0; i < 10; i++)
		buffer[10] = i;
	LocalRegionContext<uint64_t> region(buffer);

	// Making a snapshot of the current initial values
	std::vector<size_t> initialV(buffer, buffer + 10);


	// Apply successful CAS on all elements (// incrementing all elements by one)
	for (primitive::offset_t i = 0; i < 10; i++) {
//		std::promise<ErrorType> errorProm;
//		std::future<ErrorType> errorFuture;
//		std::promise<uint64_t>  actualHeadProm;
//		std::future<uint64_t> actualHeadFuture;
//
//		errorFuture = errorProm.get_future();
//		actualHeadFuture = actualHeadProm.get_future();

		uint64_t expected = initialV.at(i);
		uint64_t desired = initialV.at(i) + 1;

		// region.CAS(&expected, desired, i, errorProm, actualHeadProm);
		ErrorType e = region.CAS(&expected, desired, i);

//		ErrorType e			= errorFuture.get();
//		uint64_t actualHead	= actualHeadFuture.get();
		uint64_t actualHead	= expected;

		assert(e == error::SUCCESS);
		assert(actualHead == initialV.at(i));
	}

	// Now apply unsuccessful CAS on all elements
	for (primitive::offset_t i = 0; i < 10; i++) {
//		std::promise<ErrorType> errorProm;
//		std::future<ErrorType> errorFuture;
//		std::promise<uint64_t>  actualHeadProm;
//		std::future<uint64_t> actualHeadFuture;
//
//		errorFuture = errorProm.get_future();
//		actualHeadFuture = actualHeadProm.get_future();

		uint64_t expected = initialV.at(i);
		uint64_t desired = initialV.at(i) - 1;

		// region.CAS(&expected, desired, i, errorProm, actualHeadProm);
		ErrorType e = region.CAS(&expected, desired, i);

//		ErrorType e			= errorFuture.get();
//		uint64_t actualHead	= actualHeadFuture.get();
		uint64_t actualHead = expected;

		assert(e == error::CAS_FAILED);
		assert(actualHead == initialV.at(i) + 1);
	}

	// Now apply successful CAS on all elements (decrement all elements by one)
	for (primitive::offset_t i = 0; i < 10; i++) {
//		std::promise<ErrorType> errorProm;
//		std::future<ErrorType> errorFuture;
//		std::promise<uint64_t>  actualHeadProm;
//		std::future<uint64_t> actualHeadFuture;

//		errorFuture = errorProm.get_future();
//		actualHeadFuture = actualHeadProm.get_future();

		uint64_t expected = initialV.at(i) + 1;
		uint64_t desired = initialV.at(i);

		// region.CAS(&expected, desired, i, errorProm, actualHeadProm);
		ErrorType e = region.CAS(&expected, desired, i);

//		ErrorType e			= errorFuture.get();
//		uint64_t actualHead	= actualHeadFuture.get();

		assert(e == error::SUCCESS);
		assert(buffer[i] == initialV.at(i));
	}
}
