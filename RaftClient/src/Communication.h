#pragma once

#include <string>
#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>


#include "ClientRequest.h"

#define MSG_SUCCESS                                 0
#define MSG_ERROR_INITIALIZATION_SOCKET             1
#define MSG_ERROR_UNKNOWN_SERVER                    2
#define MSG_ERROR_CREATE_SOCKET                     3
#define MSG_ERROR_FAILED_TO_CONNECT_SERVER          4
#define MSG_ERROR_TO_ASSOCIATE_PORT_AND_IP_SOCKET   5
#define MSG_ERROR_TO_ENABLE_INGOING_CONNECTIONS     6
#define MSG_ERROR_TO_ACCEPT_INGOING_CONNECTIONS     7
#define MSG_ERROR_TO_WRITE							8

#define COMPILE_TIME_ASSERT(expr) {typedef char COMP_TIME_ASSERT[(expr) ? 1 : 0];}

#define MAX_SIZE_BUFFER								1024


class Communication
{
public: 
	Communication();
	~Communication();
	int sendMessage(ClientRequest* client_request, unsigned short port, std::string sender, std::string action, std::string receiver);
	int receiveMessage(ClientRequest* client_request, unsigned short port, std::string receiver);
	int startReceiveMessage(unsigned short port, std::string receiver);
		
private: 
	char SendBuff[MAX_SIZE_BUFFER], RecvBuff[MAX_SIZE_BUFFER];
	int  listenfd_;
	int  connfd_;
	int  connect_with_timeout(int sockfd, const struct sockaddr *addr, socklen_t addrlen, unsigned int timeout_ms);

};

