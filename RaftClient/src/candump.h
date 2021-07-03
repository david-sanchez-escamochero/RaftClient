/*
 * candump.h
 *
 *  Created on: 25/05/2021
 *      Author: pi
 */

#ifndef CANDUMP_H_
#define CANDUMP_H_

int can_dump(char **argv);
void close_connections();
void sigterm(int signo);

#endif /* CANDUMP_H_ */
