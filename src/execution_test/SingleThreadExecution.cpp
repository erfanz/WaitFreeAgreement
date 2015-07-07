/*
 *  SingleProcessExecution.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 30, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include "../agents/coordinator/Coordinator.hpp"
#include "../agents/coordinator/MemoryServerContext.hpp"
#include "../agents/memory_server/MemoryServer.hpp"
#include "../base_types/PrimitiveTypes.hpp"
#include "../config.hpp"
#include <vector>

void createMemoryServers(std::vector<MemoryServer*> &memoryServers){
	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++)
		memoryServers.push_back(new MemoryServer());

}

void createCoordinators(std::vector<Coordinator*> &coordinators) {
	for (primitive::coordinator_num_t i = 0; i < config::COORDINATOR_CNT; i++)
		coordinators.push_back(new Coordinator (i));
}

void connectCoordsToMSs(std::vector<Coordinator*> &coordinators, std::vector<MemoryServer*> &memoryServers) {
	for (primitive::coordinator_num_t c = 0; c < config::COORDINATOR_CNT; c++) {
		std::vector<MemoryServerContext> memoryServerCtxs;
		for (size_t m = 0; m < config::MEMORY_SERVER_CNT; m++) {
			MemoryServerContext* mscTmp = new MemoryServerContext (*memoryServers.at(m), true);
			memoryServerCtxs.push_back(*mscTmp);
		}
		// assign to coordinators
		coordinators.at(c)->connectToMemoryServers(memoryServerCtxs);
	}
}

void constructChange(Change **change __attribute__((unused))) {
	std::vector<Dependency> dependencies;
	int b1 = 0;
	Pointer p1 = Pointer::makePointer(0x0000000000000000);
	Dependency dep1(b1, p1);
	int b2 = 2;
	Pointer p2 = Pointer::makePointer(0x0000000000000000);
	Dependency dep2(b2, p2);
	dependencies.push_back(dep1);
	dependencies.push_back(dep2);

	std::vector<KeyValue> updates;
	Key k1("k101");
	Value v1("v101");
	Key k2("k202");
	Value v2("v202");
	KeyValue kv1 (k1, v1);
	KeyValue kv2 (k2, v2);
	updates.push_back(kv1);
	updates.push_back(kv2);

	*change = new Change(updates, dependencies);
}

int main () {
	std::vector<MemoryServer*> memoryServers;
	createMemoryServers(memoryServers);

	std::vector<Coordinator*> coordinators;
	createCoordinators(coordinators);

	connectCoordsToMSs(coordinators, memoryServers);

	Change* change;
	constructChange(&change);

	TID tid;
	tid.id = 12;
	coordinators.at(0)->applyChange(*change, tid);


	// let's see if the change is visible
	Key k1("k101");
	Value value;
	SCN scn(1);
	coordinators.at(0)->readByKey(k1, scn, tid, value);

	std::cout << "The content of key " << k1.getId() << " is " << value.getContent() << std::endl;


	// deleting objects
	for (size_t i = 0; i < config::MEMORY_SERVER_CNT; i++) {
		delete memoryServers.at(i);
	}

	for (size_t i = 0; i < config::COORDINATOR_CNT; i++) {
		delete coordinators.at(i);
	}

	return 0;
}
