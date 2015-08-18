/*
 *  EntryRWRequest.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Aug 13, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef ENTRYREADREQUEST_HPP_
#define ENTRYREADREQUEST_HPP_

#include "Request.hpp"
#include "../../base_types/LogEntry.hpp"
#include "../../base_types/PrimitiveTypes.hpp"
#include "memory"	// std::shared_ptr

typedef std::shared_ptr<LogEntry>	entry_ptr_t;
typedef std::shared_ptr<ErrorType>	prom_ptr_t;


class EntryReadRequest: public Request {
public:
	struct EntryReadParameters {
		LogEntry						&localEntry_;
		const primitive::offset_t		remoteBufferOffset_;
		const primitive::entry_size_t	length_;

		EntryReadParameters(LogEntry &localEntry, const primitive::offset_t remoteBufferOffset, const primitive::entry_size_t length):
			localEntry_(localEntry),
			remoteBufferOffset_(remoteBufferOffset),
			length_(length) {}
	};

	EntryReadRequest(err_promise_t &, const size_t memoryServerNum, const primitive::coordinator_num_t , LogEntry &, const primitive::offset_t, const primitive::entry_size_t);
	primitive::coordinator_num_t getCoordinatorID() const;
	std::shared_ptr<EntryReadParameters>& getParameters();
	~EntryReadRequest();

private:
	std::shared_ptr<EntryReadParameters>	params_;
	const primitive::coordinator_num_t			cID_;
};

#endif /* ENTRYREADREQUEST_HPP_ */
