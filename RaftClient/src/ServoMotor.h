/*
 * ServoMotor.h
 *
 *  Created on: 24/05/2021
 *      Author: pi
 */

#ifndef SERVOMOTOR_H_
#define SERVOMOTOR_H_

#define MIN 4
#define MAX 26


class ServoMotor {
public:
	ServoMotor();
	virtual ~ServoMotor();
	bool SetDegree(int degree);
	bool SetDegree_Thunderstruck(int degree);

	void Thunderstruck_loop(int x, int y, int degreeA, int degreeB, int timeout_ms);
	void Thunderstruck_shout(int degreeA, int timeout_ms);

	void final_count_down(int degreeA, int timeout_ms);
	void rythm(int degreeA);
};

#endif /* SERVOMOTOR_H_ */
