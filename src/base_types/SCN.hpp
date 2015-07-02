/*
 *  SCN.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 12, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef BASE_TYPES_SCN_HPP_
#define BASE_TYPES_SCN_HPP_

class SCN {
private:
	uint64_t id_;

public:
	SCN(uint64_t id){this->id_ = id;}
	uint64_t getId() const {return id_;}

	bool isSCNCompatible(const SCN missedSCN) const;
};

bool SCN::isSCNCompatible(const SCN missedSCN) const{
	if (missedSCN.id_ == 0 || missedSCN.id_ > this->id_)
		return true;
	else return false;
}



#endif /* BASE_TYPES_SCN_HPP_ */
