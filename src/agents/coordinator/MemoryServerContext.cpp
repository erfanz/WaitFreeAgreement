/*
 *  MemoryServerContext.cpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 11, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */
//
//MemoryServerContext::MemoryServerContext(MemoryServer memServer, bool isLocal){
//	memoryServer = memServer;
//	this->isLocal = isLocal;
//}
//
//bool MemoryServerContext::isKeyValid (const uint64_t hashedKey, const Key key) const{
//	if (isLocal){
//		SCN scn(memoryServer.keyValid);
//		// TODO
//		return true;
//
//	}
//	else {
//		// TODO
//		return false;
//	}
//}
//
//SCN MemoryServerContext::getLastMissedSCN(const HashedKey hashedKey) const{
//	return memoryServer.keyValid[hashedKey.getId()];
//}
//
//LogEntry* MemoryServerContext::findItemByVersion(HashedKey hashedKey, Key key, SCN scn, TID tid) const{
//	Pointer p = p.makePointer(memoryServer.keyHash[hashedKey.getId()]);
//	LogEntry *entry = fetchLogEntry(p.coordinatorNum, p.offset, p.length);
//	if (entry->key.isEqual(key)) {
//		// this is the item we are looking for
//		return entry;
//	}
//}
//
//LogEntry* MemoryServerContext::fetchLogEntry(uint8_t coordinatorNum, uint32_t offset, uint16_t length) const {
//	LogEntry *entry = new LogEntry();
//
//
//}

