#include "Communication.h"
#include <stdio.h>
//#include <windows.h>
#include <stdlib.h>
#include "Tracer.h"
#include "RaftUtils.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>

#define IP_SERVER_RAFT "192.168.1.22"



Communication::Communication() {

    COMPILE_TIME_ASSERT(sizeof(ClientRequest) > sizeof(MAX_SIZE_BUFFER));
	listenfd_ 	= 0;
	connfd_ 	= 0;
}

Communication::~Communication() {

	shutdown(connfd_, SHUT_RDWR);
	close(connfd_);
	Tracer::trace("Communication::~Communication() - closed connfd_\r\n");

	shutdown(listenfd_, SHUT_RDWR);
	close(listenfd_);
	Tracer::trace("Communication::~Communication() - closed listenfd_\r\n");
}


int Communication::connect_with_timeout(int sockfd, const struct sockaddr *addr, socklen_t addrlen, unsigned int timeout_ms) {
    int rc = 0;
    // Set O_NONBLOCK
    int sockfd_flags_before;
    if((sockfd_flags_before=fcntl(sockfd,F_GETFL,0)<0)) return -1;
    if(fcntl(sockfd,F_SETFL,sockfd_flags_before | O_NONBLOCK)<0) return -1;
    // Start connecting (asynchronously)
    do {
        if (connect(sockfd, addr, addrlen)<0) {
            // Did connect return an error? If so, we'll fail.
            if ((errno != EWOULDBLOCK) && (errno != EINPROGRESS)) {
                rc = -1;
            }
            // Otherwise, we'll wait for it to complete.
            else {
                // Set a deadline timestamp 'timeout' ms from now (needed b/c poll can be interrupted)
                struct timespec now;
                if(clock_gettime(CLOCK_MONOTONIC, &now)<0) { rc=-1; break; }
                struct timespec deadline = { .tv_sec = now.tv_sec,
                                             .tv_nsec = now.tv_nsec + timeout_ms*1000000l};
                // Wait for the connection to complete.
                do {
                    // Calculate how long until the deadline
                    if(clock_gettime(CLOCK_MONOTONIC, &now)<0) { rc=-1; break; }
                    int ms_until_deadline = (int)(  (deadline.tv_sec  - now.tv_sec)*1000l
                                                  + (deadline.tv_nsec - now.tv_nsec)/1000000l);
                    if(ms_until_deadline<0) { rc=0; break; }
                    // Wait for connect to complete (or for the timeout deadline)
                    struct pollfd pfds[] = { { .fd = sockfd, .events = POLLOUT } };
                    rc = poll(pfds, 1, ms_until_deadline);
                    // If poll 'succeeded', make sure it *really* succeeded
                    if(rc>0) {
                        int error = 0; socklen_t len = sizeof(error);
                        int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
                        if(retval==0) errno = error;
                        if(error!=0) rc=-1;
                    }
                }
                // If poll was interrupted, try again.
                while(rc==-1 && errno==EINTR);
                // Did poll timeout? If so, fail.
                if(rc==0) {
                    errno = ETIMEDOUT;
                    rc=-1;
                }
            }
        }
    } while(0);
    // Restore original O_NONBLOCK state
    if(fcntl(sockfd,F_SETFL,sockfd_flags_before)<0) return -1;
    // Success
    return rc;
}


int Communication::sendMessage(ClientRequest* client_request, unsigned short port, std::string sender, std::string action, std::string receiver)
{    
    int sockfd = 0;//, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;
    std::string str_trace;


    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
    	str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))   - FAILED] - (Wrong socket created)\r\n";
    	Tracer::trace(str_trace, SeverityTrace::error_trace);
    	return MSG_ERROR_CREATE_SOCKET;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, IP_SERVER_RAFT, &serv_addr.sin_addr)<=0)
    {
    	str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + ")) - FAILED] - (Error socket initialization)\r\n";
    	Tracer::trace(str_trace, SeverityTrace::error_trace);
    	return MSG_ERROR_INITIALIZATION_SOCKET;
    }


    if( connect_with_timeout(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr), 2000 ) < 0 )
    {
		str_trace = "[<<<<< Sent([" + action + "]" + sender + " -> " + receiver + "(" + std::to_string(port) + "))    - FAILED] - (Failed to connect server)\r\n";
		Tracer::trace(str_trace, SeverityTrace::error_trace);
		close(sockfd);

		return MSG_ERROR_FAILED_TO_CONNECT_SERVER;
    }


    memcpy(SendBuff, reinterpret_cast<const char *>(client_request), sizeof(ClientRequest));

    if(write(sockfd, SendBuff, sizeof(ClientRequest)) == -1){
    	Tracer::trace("Failed to write in socket\r\n", SeverityTrace::error_trace);
    	return MSG_ERROR_TO_WRITE;
    }


    close(sockfd);

    return EXIT_SUCCESS;
}


int Communication::startReceiveMessage(unsigned short port, std::string receiver)
{
	std::string str_trace;
    int n = 0;
    struct sockaddr_in serv_addr;


    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    Tracer::trace("START CONNECTION 1\r\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if(bind(listenfd_, (struct sockaddr*)&serv_addr, sizeof(serv_addr))){
    	str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] (Failed to bind server)\r\n";
    	Tracer::trace(str_trace, SeverityTrace::error_trace);
    	return MSG_ERROR_TO_ASSOCIATE_PORT_AND_IP_SOCKET;
    }
    Tracer::trace("START CONNECTION 2\r\n");

    if(listen(listenfd_, 10)){
    	str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] (Failed to listen server)\r\n";
    	Tracer::trace(str_trace, SeverityTrace::error_trace);
    	return MSG_ERROR_TO_ENABLE_INGOING_CONNECTIONS;
    }
    Tracer::trace("START CONNECTION 3\r\n");

    return (EXIT_SUCCESS);
}


int Communication::receiveMessage(ClientRequest* client_request, unsigned short port, std::string receiver)
{
	std::string str_trace;
	char recvBuff[1025];
	memset(recvBuff, '0', sizeof(recvBuff));


printf("BEFORE ACCEPT\r\n");
    connfd_ = accept(listenfd_, (struct sockaddr*)NULL, NULL);
    if(connfd_ == -1){
		str_trace = "[>>>>> Received(" + receiver + "(" + std::to_string(port) + ")) - FAILED] (Failed to accept ingoing conection)\r\n";
		Tracer::trace(str_trace, SeverityTrace::error_trace);
		return MSG_ERROR_TO_ACCEPT_INGOING_CONNECTIONS;
    }
printf("AFTER ACCEPT\r\n");

    size_t total_bytes = 0;
    int size = sizeof(ClientRequest);
	while(total_bytes < size) {
		ssize_t new_bytes = read(connfd_, recvBuff + total_bytes, size - total_bytes);
		if(new_bytes > 0) {
			total_bytes += new_bytes;
		} else if(new_bytes == 0 || (new_bytes == -1 && errno != EINTR)) {
			return false;
		}
	}

	ClientRequest* client_request_aux = reinterpret_cast<ClientRequest*>(recvBuff);
	*client_request = *client_request_aux;

    close(connfd_);


    return (EXIT_SUCCESS);
}




