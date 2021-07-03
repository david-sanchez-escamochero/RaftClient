/*
 * ServoMotor.cpp
 *
 *  Created on: 24/05/2021
 *      Author: pi
 */

#include "ServoMotor.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <string>
#include "Tracer.h"
#include <thread>
#include "RaftUtils.h"

using namespace std;

extern "C" {
	#include "cansend.h"
	#include "candump.h"
}



#define P_PIN 0// PIN 11 de la RPI


ServoMotor::ServoMotor() {
	// TODO Auto-generated constructor stub
	//printf("Starting...:%d\r\n", P_PIN);
	wiringPiSetup();
	pinMode(P_PIN, PWM_OUTPUT);
	digitalWrite(P_PIN, 0);
	softPwmCreate(P_PIN, 0, 200);

}

ServoMotor::~ServoMotor() {
	// TODO Auto-generated destructor stub
}

bool ServoMotor::SetDegree(int degree)
{
	Tracer::trace("Set PIN: " + std::to_string(P_PIN) + " servo motor degree:" + std::to_string(degree) + "\r\n");
	softPwmWrite(P_PIN, degree);
	return true;
}

bool ServoMotor::SetDegree_Thunderstruck(int degree)
{
	char device[] = "can0";
	char can_frame[] = "003#000000000000090F";

	Tracer::trace("Set PIN: " + std::to_string(P_PIN) + " servo motor degree:" + std::to_string(degree) + "\r\n");
	softPwmWrite(P_PIN, degree);

	RaftUtils::from_degrees_to_frame(can_frame, degree);
	// Can send
	int can_send_error = can_send(device, can_frame);

	if(can_send_error){
		Tracer::trace("Client - can send FAILED!!! error: " + std::to_string(can_send_error) + "\r\n");
	}
	return true;
}


void ServoMotor::Thunderstruck_loop(int x, int y, int degreeA, int degreeB, int timeout_ms)
{
	for(int count = x; count < y; count++){
		if( (count % 2) == 0)
			SetDegree_Thunderstruck(degreeA);
		else
			SetDegree_Thunderstruck(degreeB);

		std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
	}

}
void ServoMotor::Thunderstruck_shout(int degreeA, int timeout_ms){
	SetDegree_Thunderstruck(degreeA);
	std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
}

void ServoMotor::final_count_down(int degreeA, int timeout_ms){
	SetDegree_Thunderstruck(degreeA);
	std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
}


void ServoMotor::rythm(int degreeA){
	SetDegree_Thunderstruck(degreeA);
}
