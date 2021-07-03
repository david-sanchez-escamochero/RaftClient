#include "Client.h"
#include <iostream>
#include <string>
#include <fstream>
#include "ClientRequest.h"
#include "ClientDefs.h"
#include "RaftDefs.h"
#include <wiringPi.h>
#include "IPManager.h"
#include <cstring>
#include "TimeManager.h"
#include "ServoMotor.h"
#include "RaftUtils.h"
#include <chrono>
//#include "cansend.h"
//#include "candump.h"

extern "C" {
	#include "cansend.h"
	#include "candump.h"
}


using namespace std;


Client::Client()
{
    have_to_die_ 			= false;
    receive_have_to_die_  	= false;
    leader_      			= NONE;
    communication_ 			= new Communication();
}

Client::~Client()
{
    have_to_die_ 			= true;
    receive_have_to_die_  	= true;
    if(communication_ != NULL)
    	delete(communication_);


    Tracer::trace("Client::~Client 1 \r\n");
    if (thread_server_receive_leader_.joinable()){
        thread_server_receive_leader_.join();
    }
    Tracer::trace("Client::~Client 2 \r\n");
}

bool Client::start(int32_t client_id)
{
    bool ret    = true;
    client_id_  = client_id;

    communication_->startReceiveMessage(SOCKET_BASE_PORT + SOCKET_RECEIVER_PORT + client_id_, LEADER_TEXT);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // Start server receive leader
    thread_server_receive_leader_ = std::thread(&Client::receive, this);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));


    /*find_a_leader();

    send_write_master(leader_);

    if(master_ == client_id){
    	Tracer::trace("I am a master: " + std::to_string(master_) + "\r\n");
    	client_i_am_master();
    }
    else{
    	Tracer::trace("I am a follower:" + std::to_string(client_id) + " (master:" + std::to_string(master_) + ")\r\n" );
    	client_i_am_follower();
    }*/


    return ret;
}


bool Client::master_or_follower(int32_t client_id){
	bool ret    			= true;
    have_to_die_ 			= false;
    receive_have_to_die_  	= false;
    leader_      			= NONE;


    find_a_leader();

    send_write_master(leader_);

    if(master_ == client_id){
    	Tracer::trace("I am a master: " + std::to_string(master_) + "\r\n");
    	client_i_am_master();
    }
    else{
    	Tracer::trace("I am a follower:" + std::to_string(client_id) + " (master:" + std::to_string(master_) + ")\r\n" );
    	client_i_am_follower();
    }


    return ret;
}


void Client::find_a_leader() {
    bool ret = false;
    leader_  = NONE;

    uint32_t num_server = 0;

    do {
        send_request_to_find_a_leader(num_server++);
        {
            // Time waiting servers replay saying who is the leader...
            Tracer::trace("Client - Leader does not respond, error: " + std::to_string(ret) + "\r\n", SeverityTrace::error_trace);
            Tracer::trace("Client - Waiting " + std::to_string(TIME_WAITING_A_LEADER) + "(ms) to retry...\r\n");
            std::mutex mtx;
            std::unique_lock<std::mutex> lck(mtx);
            cv_found_a_leader_.wait_for(lck, std::chrono::milliseconds(TIME_WAITING_A_LEADER));
        }

        if (num_server == NUM_SERVERS)
            num_server = 0;

    } while (leader_ == NONE);
}

void* Client::receive()
{
    while (!receive_have_to_die_) {
        ClientRequest client_request;
        Tracer::trace("receive 1 have_to_die_:"+ std::to_string(have_to_die_)+"\r\n");
        int error = communication_->receiveMessage(&client_request, SOCKET_BASE_PORT + SOCKET_RECEIVER_PORT + client_id_, LEADER_TEXT);
        Tracer::trace("receive 2 have_to_die_:"+ std::to_string(have_to_die_)+" \r\n");

        if (error) {
            Tracer::trace("Client::receive - FAILED!!!  - error" + std::to_string(error) + "\r\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }
        else {
            if (
                (client_request.client_request_type == ClientRequesTypeEnum::client_request_leader) &&
                (client_request.client_result_ == (uint32_t)true)
                ) {
					if (client_request.client_leader_ == NONE) {
						Tracer::trace("Client - I do not know yet who is the leader\r\n", SeverityTrace::action_trace);
					}
					else {
						leader_ = client_request.client_leader_;
						Tracer::trace("Client - I have found a Leader: " + std::to_string(leader_) + "\r\n", SeverityTrace::action_trace);
						cv_found_a_leader_.notify_one();
					}
            }
            else if (
                (client_request.client_request_type == ClientRequesTypeEnum::client_write_master) &&
                (client_request.client_result_ == (uint32_t)true)
                ) {
                Tracer::trace("Client - I know who is the master: " +std::to_string(client_request.client_master_) +  "\r\n", SeverityTrace::action_trace);
            	master_ = client_request.client_master_;
                cv_committed_value_.notify_one();
            }
        }
    }
    return nullptr;
}

void Client::send_request_to_find_a_leader(uint32_t num_server) {
    
    ClientRequest client_request;
    IPManager ip_manager;
    char ip[80];
    ip_manager.getIP(ip);

    client_request.client_request_type = ClientRequesTypeEnum::client_request_leader;
    client_request.client_id_ = client_id_;
    strcpy(client_request.client_ip_, ip);
    client_request.client_seconds_january_1_1970 = TimeManager::getSecondsFrom_January_1_1970();
    Tracer::trace("Client -  seconds_january_1_1970:" + std::to_string(client_request.client_seconds_january_1_1970) + "\r\n");

    int ret = NONE;

    ret = communication_->sendMessage(&client_request, SOCKET_BASE_PORT + SOCKET_RECEIVER_PORT + num_server, CLIENT_TEXT, CLIENT_REQUEST_LEADER_TEXT, LEADER_TEXT);
    if (ret) {
        Tracer::trace("Client - Failed to request leader from server " + std::to_string(num_server) + ", error " + std::to_string(ret) + "\r\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int Client::send_write_master(uint32_t leader_id)
{
    IPManager ip_manager;
    char ip[80];
    ip_manager.getIP(ip);

	Tracer::trace("Client - Value proposed:" + std::to_string(client_id_) + "\r\n", SeverityTrace::action_trace);


	ClientRequest client_request;
	client_request.client_request_type = ClientRequesTypeEnum::client_write_master;
	client_request.client_id_ = client_id_;
	client_request.client_value_ = client_id_; // El valor es el ID del client(Master)
	strcpy(client_request.client_ip_, ip);
	client_request.client_seconds_january_1_1970 = TimeManager::getSecondsFrom_January_1_1970();
	Tracer::trace("Client - send write a master - seconds_january_1_1970:" + std::to_string(client_request.client_seconds_january_1_1970) + "\r\n");
	cv_status time_out;

	do {
		int ret = 0;
		do
		{
			int ret = communication_->sendMessage(&client_request, SOCKET_BASE_PORT + SOCKET_RECEIVER_PORT + leader_id, CLIENT_TEXT, CLIENT_REQUEST_LEADER_TEXT, LEADER_TEXT);
			if (ret) {
				Tracer::trace("Client - Leader does not respond, error: " + std::to_string(ret) + "\r\n");
			}

		} while (ret);
		{

			// Time waiting servers replay if value is commited...
			std::mutex mtx;
			std::unique_lock<std::mutex> lck(mtx);
			Tracer::trace("===> Waiting for the master\r\n");
			time_out = cv_committed_value_.wait_for(lck, std::chrono::milliseconds(TIME_WAITING_COMMIT_VALUE));
			Tracer::trace("===> OK Waiting for the master\r\n");
		}
		if (time_out == std::cv_status::timeout) {
			find_a_leader();
		}
	} while (time_out == std::cv_status::timeout);


    return master_;
}

void Client::ping()
{
	// Ping.
	IPManager ip_manager;
	char ip[80];
	ip_manager.getIP(ip);

	ClientRequest client_request;
	client_request.client_request_type = ClientRequesTypeEnum::client_ping_master;
	client_request.client_id_ = client_id_;
	client_request.client_value_ = client_id_; // El valor es el ID del client(Master)
	strcpy(client_request.client_ip_, ip);
	client_request.client_seconds_january_1_1970 = TimeManager::getSecondsFrom_January_1_1970();


	int ret = communication_->sendMessage(&client_request, SOCKET_BASE_PORT + SOCKET_RECEIVER_PORT + leader_, CLIENT_TEXT, CLIENT_REQUEST_LEADER_TEXT, LEADER_TEXT);
	if (ret) {
		Tracer::trace("Client - Leader does not respond, error: " + std::to_string(ret) + "\r\n");
	}
}


void  Client::client_i_am_master()
{
	ServoMotor servo_motor;

	IPManager ip_manager;
	char ip[80];
	ip_manager.getIP(ip);


	Tracer::trace("Client - I am a master\r\n",SeverityTrace::action_trace);

	while(!have_to_die_){
		// Move servo...

		if( (master_ % 2) == 0) {

			/*0s*/servo_motor.Thunderstruck_loop(1, 29*5, 6, 10, 200);
			/*29s*/servo_motor.Thunderstruck_shout(24, 1500);

			ping();

			/*30,5s*/servo_motor.Thunderstruck_loop(1, 2*4, 6, 10, 200);
			/*32s*/servo_motor.Thunderstruck_shout(24, 1700);
			/*33,5s*/servo_motor.Thunderstruck_loop(1, (2*5) + 1, 6, 10, 200);
			/*36s*/servo_motor.Thunderstruck_shout(24, 1700);
			/*37,5s*/servo_motor.Thunderstruck_loop(1, (2*5) + 2, 6, 10, 200);
			/*40s*/servo_motor.Thunderstruck_shout(24, 1700);
			/*41,5s*/servo_motor.Thunderstruck_loop(1, (2*6) - 1, 6, 10, 200);
			/*43s*/servo_motor.Thunderstruck_shout(24, 1700);
			/*44,5s*/servo_motor.Thunderstruck_loop(1, (2*6) - 1, 6, 10, 200);
			/*47s*/servo_motor.Thunderstruck_shout(24, 1700);
			/*48,5s*/servo_motor.Thunderstruck_loop(1, (2*6) - 1, 6, 10, 200);
			/*50s*/servo_motor.Thunderstruck_shout(24, 1700);
			/*51,5s*/servo_motor.Thunderstruck_loop(1, (3*5) + 1, 6, 10, 200);
			/*54s*/servo_motor.Thunderstruck_shout(24, 1700);
			/*55,5s*/servo_motor.Thunderstruck_loop(1, (2*6) - 1, 6, 10, 200);
			/*58s*/servo_motor.Thunderstruck_shout(24, 500);
			/*58,5s*/servo_motor.Thunderstruck_loop(1, 2*6, 6, 10, 200);

			ping();

			/*1.01s*/servo_motor.Thunderstruck_shout(24, 1500);
			/*1.01,5s*/servo_motor.Thunderstruck_loop(1, 9*5, 6, 10, 200);
			/*1.10s*/servo_motor.Thunderstruck_shout(24, 1500);
			/*1.10.5s*/servo_motor.Thunderstruck_loop(1, 7*5, 6, 10, 200);
			/*1.17.5s*/servo_motor.Thunderstruck_shout(24, 1500);
			/*1.18.5s*/servo_motor.Thunderstruck_loop(1, 7*5, 6, 10, 200);
			/*1.25s*/servo_motor.Thunderstruck_shout(24, 1500);
			/*1.26.5s*/servo_motor.Thunderstruck_loop(1, 6*5, 6, 10, 200);

			ping();

			/*1.32s*/servo_motor.Thunderstruck_shout(24, 1500);
			/*1.33.5*/servo_motor.Thunderstruck_loop(1, 18*5, 6, 10, 200);
			/*1.51s*/servo_motor.Thunderstruck_shout(24, 1500);

			ping();
		}
		else {

			servo_motor.final_count_down(4, 200);
			servo_motor.final_count_down(7, 200);
			servo_motor.final_count_down(4, 200);
			servo_motor.final_count_down(15, 1000);
			servo_motor.final_count_down(4, 200);
			servo_motor.final_count_down(7, 200);
			servo_motor.final_count_down(4, 200);
			servo_motor.final_count_down(15, 1000);
			servo_motor.final_count_down(4, 200);
			servo_motor.final_count_down(7, 200);
			servo_motor.final_count_down(4, 200);
			servo_motor.final_count_down(15, 2000);

			ping();
		}

	}
	Tracer::trace("Client - Master finished\r\n",SeverityTrace::action_trace);
}

void  Client::client_i_am_follower()
{
	Tracer::trace("Client - I am a follower\r\n",SeverityTrace::action_trace);
	char *arg[2];
	char cand_dump_str[] = "candump";
	char device_str[] = "can0";
	arg[0] = cand_dump_str;
	arg[1] = device_str;

	//ServoMotor servo_motor;
	thread_check_if_frames_are_received_ = std::thread(&Client::check_if_frames_are_received, this);

	while(!have_to_die_){
		// cand dump

   		int degrees = can_dump(arg);
   		// If there is not connection...
   		if( ( degrees >= 4 ) && ( degrees <= 26 ) ) {
			//servo_motor.SetDegree(degrees);
			Tracer::trace("Client(Follower) - Degrees: " + std::to_string(degrees) + "(s)\r\n");
			last_time_stam_taken_miliseconds_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			seconds_without_receiveng_frames_ = 0;
   		}
	}
	Tracer::trace("Client - Follower finished\r\n",SeverityTrace::action_trace);

	if(thread_check_if_frames_are_received_.joinable()){

		thread_check_if_frames_are_received_.join();
		Tracer::trace("Client(Follower) - thread_check_if_frames_are_received_ finished\r\n");
	}
}

void Client::check_if_frames_are_received()
{
	seconds_without_receiveng_frames_ = 0;
	last_time_stam_taken_miliseconds_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	while (!have_to_die_) {


		milliseconds current_time_stam_taken_miliseconds = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		if ((abs(last_time_stam_taken_miliseconds_.count() - current_time_stam_taken_miliseconds.count())) > TIME_OUT_RECEIVE_FRAMES)
		{
			sigterm(0);
			have_to_die_ = true;
		}
		Tracer::trace("Client(Follower) - Seconds without receiving frames: " + std::to_string(seconds_without_receiveng_frames_++) + "(s)\r\n");

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
