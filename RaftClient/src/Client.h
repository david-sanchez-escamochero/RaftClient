#pragma once

#include <string>

#include "Tracer.h"
#include <thread>
#include "Semaphore.h"
#include "Communication.h"

using namespace std::chrono;

class Client
{
public:
	Client();
	~Client();
	bool					start(int32_t client_id);
	bool 					master_or_follower(int32_t client_id);
private:
	void					send_request_to_find_a_leader(uint32_t num_server);
	int						send_write_master(uint32_t leader_id);
	void*					receive();
	bool					have_to_die_;
	bool					receive_have_to_die_;
	Communication*			communication_;
	void 					check_if_frames_are_received();

	int32_t					leader_;
	int32_t 				master_;
	std::thread				thread_server_receive_leader_;
	std::thread				thread_check_if_frames_are_received_;
	Semaphore				semaphore_wait_reveive_leader_;
	Semaphore				semaphore_wait_commit_value_;
	void					find_a_leader();
	std::condition_variable cv_found_a_leader_;
	std::condition_variable cv_committed_value_;
	uint32_t				client_id_;
	void 					client_i_am_master();
	int 					seconds_without_receiveng_frames_;
	void 					ping();
public:
	void 					client_i_am_follower();
	milliseconds 			 last_time_stam_taken_miliseconds_;

};

