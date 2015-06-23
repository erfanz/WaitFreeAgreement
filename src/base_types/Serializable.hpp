/*
 *  Serializable.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 15, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#ifndef SERIALIZABLE_HPP_
#define SERIALIZABLE_HPP_

#include <iostream>      // std::ios, std::istream, std::cout

template <typename T>
class Serializable {
public:
    virtual ~Serializable(){}

    virtual void serialize(std::ostream& stream) const = 0;
    static void deserialize(std::istream& stream, T &t) {
    	T::doDeserialize(stream, t);
    };
};


#endif /* SERIALIZABLE_HPP_ */
