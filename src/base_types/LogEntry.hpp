/*
 * LogEntry.hpp
 *
 *  Created on: Jun 9, 2015
 *      Author: Erfan Zamanian
 */

#ifndef LOGENTRY_HPP_
#define LOGENTRY_HPP_

#include "Serializable.hpp"
#include "KeyValue.hpp"
#include "Pointer.hpp"
#include "Dependency.hpp"


#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <sstream>      // std::ostringstream, std::istringstream
#include <iostream>     // std::ios, std::istream, std::cout

class LogEntry : public Serializable<LogEntry>{
private:
	std::vector<Dependency> dependencies;
	std::vector<KeyValue> updates;
	Pointer currentP;
	bool serialized;


public:
	LogEntry();
	LogEntry(std::vector<Dependency>, std::vector<KeyValue> updates, Pointer currentP, bool serialized);

	const std::vector<Dependency>& getDependencies() const;
	const std::vector<KeyValue>& getUpdates() const;
	const Pointer getCurrentP() const;
	void setCurrentP(const Pointer& currentP);
	void setDependencies(const std::vector<Dependency>& dependencies);
	void setSerialized(bool serialized);
	void setUpdates(const std::vector<KeyValue>& updates);
	void setAll(const std::vector<Dependency>& dependencies, const std::vector<KeyValue>& updates, const Pointer& currentP, bool serialized);


	static entry_size_t calculateEntrySize(const std::vector<Dependency> &dependencies, const std::vector<KeyValue> &updates);
	bool isSerialized() const;
	bool isEqual(const LogEntry &entry) const;
	virtual void serialize(std::ostream& stream) const;
	static void doDeserialize(std::istream&, LogEntry &);

};

#endif /* LOGENTRY_HPP_ */
