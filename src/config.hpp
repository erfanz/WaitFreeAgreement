/*
 *  config.hpp
 *
 *  Author		: Erfan Zamanian
 *  Created on	: Jun 10, 2015
 *  Description : PLEASE FILL IN THIS PART 
 *
 */

#include <string>
#include <iostream>

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#define DEBUG_ENABLED (false)

static const int	MEMORY_SERVER_CNT	= 3;
static const int	COORDINATOR_CNT 	= 3;


static const int	OPERATION_CNT 		= 100;

static const int	ITEM_CNT			= 10;		// Number of Items
static const int	LOG_JOURNAL_SIZE	= 2000;			// in bytes
static const int	HASH_SIZE			= 5;

static const int	TIMEOUT_IN_MS		= 500;		/* ms */


#endif /* CONFIG_HPP_ */