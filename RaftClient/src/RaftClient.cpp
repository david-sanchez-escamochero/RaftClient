//============================================================================
// Name        : RaftClient.cpp
// Author      : David
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "Communication.h"
#include "ClientRequest.h"
#include "Tracer.h"
#include "Client.h"
#include "ServoMotor.h"
#include "IPManager.h"
#include <stdio.h>
#include <time.h>
#include "TimeManager.h"
#include <iostream>
#include <stdlib.h>
#include <thread>
#include <chrono>



extern "C" {
	#include "cansend.h"
	#include "candump.h"
}


using namespace std;

int main(int argc, char *argv[]) {
	Tracer::trace("*************************\r\n");
	Tracer::trace("   Starting RaftClient\r\n");
	Tracer::trace("************************\r\n");



//	int status = system("/home/pi/workspace/RaftClient/Debug/scriptWeWillRockYou.sh &");
//
//	FILE *fp;
//	char path[75];
//
//	ServoMotor servo_motor;
//
//	//for(int x = 0; x < 4000; x++){
//	while(true){
//
//		int count_number_of_equals = 4;
//		/* Open the command for reading. */
//		fp = popen("tail -1 log.txt", "r");
//		if (fp == NULL) {
//			printf("Failed to run command\n" );
//			exit(1);
//		}
//		/* Read the output a line at a time - output it. */
//		while (fgets(path, sizeof(path), fp) != NULL) {
//			//printf("%s", path);
//		}
//
//		//printf(">>>>>>> %s\r\n", path);
//		for(int count = 0; count < 75; count ++){
//			//In:0.98% 00:00:02.88 [00:04:49.56] Out:127k
//			if(path[count]  == '=')
//				count_number_of_equals = count_number_of_equals + 1;
//			if(path[count]  == '-')
//				count_number_of_equals = count_number_of_equals + 1;
//
//		}
//		//Tracer::trace("Count number of equals: " +std::to_string(count_number_of_equals)+ "\r\n" );
//
//		/* close */
//		pclose(fp);
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//		servo_motor.rythm(count_number_of_equals);
//
//	}








// Can send
//	char device[] = "can0";
//	char can_frame[] = "003#000000000000090F";
//	can_send(device, can_frame);

// Servo motor
//	ServoMotor servo_motor;
//	while(true){
//	servo_motor.final_count_down(4, 200);
//	servo_motor.final_count_down(7, 200);
//	servo_motor.final_count_down(4, 200);
//	servo_motor.final_count_down(15, 1000);
//	servo_motor.final_count_down(4, 200);
//	servo_motor.final_count_down(7, 200);
//	servo_motor.final_count_down(4, 200);
//	servo_motor.final_count_down(15, 1000);
//	servo_motor.final_count_down(4, 200);
//	servo_motor.final_count_down(7, 200);
//	servo_motor.final_count_down(4, 200);
//	servo_motor.final_count_down(15, 1000);
//	}


//	/*0s*/servo_motor.Thunderstruck_loop(1, 29*5, 6, 10, 200);
//	/*29s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*30,5s*/servo_motor.Thunderstruck_loop(1, 2*4, 6, 10, 200);
//	/*32s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*33,5s*/servo_motor.Thunderstruck_loop(1, (2*5)+2, 6, 10, 200);
//	/*36s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*37,5s*/servo_motor.Thunderstruck_loop(1, (2*5)+3, 6, 10, 200);
//	/*40s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*41,5s*/servo_motor.Thunderstruck_loop(1, 2*6, 6, 10, 200);
//	/*43s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*44,5s*/servo_motor.Thunderstruck_loop(1, 2*6, 6, 10, 200);
//	/*47s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*48,5s*/servo_motor.Thunderstruck_loop(1, 2*6, 6, 10, 200);
//	/*50s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*51,5s*/servo_motor.Thunderstruck_loop(1, (3*5)+2, 6, 10, 200);
//	/*54s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*55,5s*/servo_motor.Thunderstruck_loop(1, 2*6, 6, 10, 200);
//	/*58s*/servo_motor.Thunderstruck_shout(24, 500);
//	/*58,5s*/servo_motor.Thunderstruck_loop(1, 2*6, 6, 10, 200);
//	/*1.01s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*1.01,5s*/servo_motor.Thunderstruck_loop(1, 9*5, 6, 10, 200);
//	/*1.10s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*1.10.5s*/servo_motor.Thunderstruck_loop(1, 7*5, 6, 10, 200);
//	/*1.17.5s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*1.18.5s*/servo_motor.Thunderstruck_loop(1, 7*5, 6, 10, 200);
//	/*1.25s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*1.26.5s*/servo_motor.Thunderstruck_loop(1, 6*5, 6, 10, 200);
//	/*1.32s*/servo_motor.Thunderstruck_shout(24, 1500);
//	/*1.33.5*/servo_motor.Thunderstruck_loop(1, 18*5, 6, 10, 200);
//	/*1.51s*/servo_motor.Thunderstruck_shout(24, 1500);




// cand dump
//   char *arg[2];
//   char cand_dump_str[] = "candump";
//   char device_str[] = "can0";
//   arg[0] = cand_dump_str;
//   arg[1] = device_str;
//   int degrees = can_dump(arg);


// get ip
//	char ip[80];
//	IPManager ip_manager;
//	ip_manager.getIP(ip);


// get time

//  time_t rawtime;
//  struct tm * timeinfo;
//
//  time ( &rawtime );
//  timeinfo = localtime ( &rawtime );
//  printf ( "Current local time and date: %s", asctime (timeinfo) );
	//long seconds = TimeManager::getSecondsFrom_January_1_1970();
	int countRafts = 0;

	Client client;
	if (!client.start(std::stoi(argv[1])))
		Tracer::trace("RaftClient - Failed to start. \r\n");


	while(true){
		Tracer::trace("RaftClient try:" + std::to_string(countRafts) + "\r\n");
		{

			client.master_or_follower(std::stoi(argv[1]));
			//client.client_i_am_follower();
			//printf("OK\r\n");


			Tracer::trace("END Client:  " + std::to_string(countRafts) + "\r\n");
		}

		for(int count = 0; count < 10; count++) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			Tracer::trace("Counter: " + std::to_string(count)+ "\r\n");
		}

		countRafts++;
	}
    return 0;
}




