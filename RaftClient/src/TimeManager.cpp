/*
 * TimeManager.cpp
 *
 *  Created on: 25/05/2021
 *      Author: pi
 */

#include "TimeManager.h"
#include <stdio.h>
#include <time.h>



TimeManager::TimeManager() {
	// TODO Auto-generated constructor stub

}

TimeManager::~TimeManager() {
	// TODO Auto-generated destructor stub
}

long TimeManager::getSecondsFrom_January_1_1970(){
	time_t seconds;
	seconds = time(NULL);
#ifdef DEBUG_TIME_MANAGER
	printf("%ld seconds since January 1, 1970\r\n", seconds);
#endif
	return seconds;
}
