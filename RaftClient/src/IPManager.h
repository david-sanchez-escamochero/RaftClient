/*
 * IPManager.h
 *
 *  Created on: 25/05/2021
 *      Author: pi
 */

#ifndef IPMANAGER_H_
#define IPMANAGER_H_

class IPManager {
public:
	IPManager();
	virtual ~IPManager();
	void getIP(char *ip);
};

#endif /* IPMANAGER_H_ */
