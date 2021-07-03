/*
 * TimeManager.h
 *
 *  Created on: 25/05/2021
 *      Author: pi
 */

#ifndef TIMEMANAGER_H_
#define TIMEMANAGER_H_

class TimeManager {
public:
	TimeManager();
	virtual ~TimeManager();
	static long getSecondsFrom_January_1_1970();
};

#endif /* TIMEMANAGER_H_ */
