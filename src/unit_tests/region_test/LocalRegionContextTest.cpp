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

#include <assert.h>
#include <string>

#define CLASS_NAME	"LocalRegionContextTest"

std::vector<std::function<void()>> LocalRegionContextTest::functionList {
	test_read_and_write,
	test_read_and_write_with_logentry
};

std::vector<std::function<void()>>& LocalRegionContextTest::getFunctionList() {
	//return PointerTest::functionList;
	return functionList;
}

void LocalRegionContextTest::test_read_and_write() {
	TestBase::printMessage(CLASS_NAME, __func__);
	char buffer[64];
	LocalRegionContext region(buffer);

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
	char buffer[1024];
	LocalRegionContext region(buffer);

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
