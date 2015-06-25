/*
 *  MemoryServerContext.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

MemoryServerContext::MemoryServerContext(MemoryServer memServer, bool isLocal){
	memoryServer = memServer;
	this->isLocal = isLocal;
}

bool MemoryServerContext::isKeyValid (const uint64_t hashedKey, const Key key) const{
	if (isLocal){
		SCN scn(memoryServer.keyValid);
		// TODO
		return true;

	}
	else {
		// TODO
		return false;
	}
}

int MemoryServerContext::getLastMissedSCN(const HashMaker hashedKey, SCN &outputScn) const{
	outputScn = memoryServer.keyValid[hashedKey.getHashed()];
	return 0;
}

int MemoryServerContext::findItemByVersion(HashMaker hashedKey, Key key, SCN scn, TID tid, LogEntry &output_entry) const{
	Pointer p;
	getKeyHash(hashedKey.getHashed(), Pointer &p);
	Pointer p = p.makePointer(memoryServer.keyHash[hashedKey.getHashed()]);

	LogEntry *entry = fetchLogEntry(p.coordinatorNum, p.offset, p.length);
	if (entry->key.isEqual(key)) {
		// this is the item we are looking for
		return entry;
	}
	return 0;
}

LogEntry* MemoryServerContext::fetchLogEntry(uint8_t coordinatorNum, uint32_t offset, uint16_t length) const {
	LogEntry *entry = new LogEntry();


}

int getKeyHash(HashMaker &, Pointer &p);


