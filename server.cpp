#include <iostream>
#include <array>
#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "parsing.h"
#include "constants.h"

struct Server {
	char protocol;
	char *host;
	int port;
} server;


int sock(const int SOCK_TYPE, const int IPPROTO){
	int masterSocket = socket(AF_INET, SOCK_TYPE, IPPROTO);
	
	if (masterSocket < 0) {
		std::cout << "Socket was not created!\n";
		return -1;
	}


	struct sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(server.port);
		
	int ip = inet_aton(server.host, &sockAddr.sin_addr);	
	if(0 == ip) {
		std::cout << "Bad host!\n";
		return -1;
	}

	if (bind(masterSocket, (struct sockaddr*)(&sockAddr), sizeof(sockAddr)) < 0) {
		std::cout << "Bind failed!\n";
		return -1;
	} 

	return masterSocket;
}

int process_and_print(char ch) {
	static int sum = 0;	
	static std::array<int, 10> arr;

	static int exit_counter = 0;
	static int cur_msg_size = 0;

	

	if('\n' ==  ch) exit_counter++;
	else exit_counter = 0;

	if (2 == exit_counter){
		sum = 0;
		arr.fill(0);
		exit_counter = 0;
		return -1;
	}	
	

	if (0 == cur_msg_size) {
		std::cout << "\nMessage:";
	}
	cur_msg_size++;

	
	// check '0' - '9'
	if ('0' <= ch && '9' >= ch) {
		sum += ch - '0';
		arr[ch - '0']++;
	}	

	// if end of message
	if ('\n' == ch) {
		std::cout << "\nSum:" << sum;
		sum = 0;

		int max = -1, min = 10;
		std::cout << "\nSorted:";

		for (int i = 9; i >= 0; i--) {
			if (arr[i] > 0) {
				max = (i > max) ? i : max;
				min = (i < min) ? i : min;
			}
			
			for (int j = 0; j < arr[i]; j++) {
				std::cout << i;
			}
		}
		arr.fill(0);

		std::cout << "\nMax:";
		if (max < 0) std::cout << "NO";
		else std::cout << max;

		std::cout << "\nMin:";
		if (min > 9) std::cout << "NO";
		else std::cout << min;

		std::cout << "\n";

		//std::cout << "\n\nMessage:";
		cur_msg_size = 0;
	} else {
		std::cout << ch;
	}
	
	return 0;
}

int main(int argc, char **argv) {
	
	std::cout << "Server is running...\n";
	
	// default settings
	server.protocol = _TCP_;
	server.host = _LOCAL_HOST_;
	server.port = 12345;

	// get setting from command line's arguments
	if (!set_settings(argc, argv, &server.protocol, &server.host, &server.port)) return -1;
	
	
	std::cout << "Server. Protocol: " << ((_TCP_ == server.protocol) ? "TCP" : "UDP") 
			<< ", Host: " << server.host 
			<< ", Port: " << server.port 
			<< ".\n";

	// additional variable
	char buf = 0;
	int msg_size = 0;


	//create and start TCP server
	if (_TCP_ == server.protocol) {
		
		int masterSocket = sock(SOCK_STREAM, IPPROTO_TCP);
		if (masterSocket < 0) return -1;

		if (listen(masterSocket, SOMAXCONN) < 0) {
			std::cout << "Listen failed!\n";
			return -1;
		}


		while (true) {
			
			int slaveSocket = accept(masterSocket, NULL, NULL);
			if (slaveSocket < 0) {
				std::cout << "Accept failed!\n";
				return -1;
			}			

			std::cout << "\nClient is connected.";
			std::cout << "\n";			
	
			do {

				msg_size = recv(slaveSocket, &buf, sizeof(buf), MSG_NOSIGNAL);
				if (msg_size < 0) {
					std::cout << "Receive failed!\n";
					return -1; //? shutdown and close?
				}
				
				if (process_and_print(buf) < 0) {
					buf = '\0';
					msg_size = 0;
					std::cout << "Connection is closing\n";
				};
	
				if (send(slaveSocket, &buf, sizeof(buf), MSG_NOSIGNAL) < 0) {
					std::cout << "Send failed!\n";
					return -1;
				}
			} while (msg_size > 0);

			if (shutdown(slaveSocket, SHUT_RDWR) < 0) {
				std::cout << "Shutdown failed!\n";
				return -1;
			}
			close(slaveSocket);
			std::cout << "Connection is closed.\n" << std::endl;	
		}

		close(masterSocket);
		return 0;	
	}

	// UDP 
	if (_UDP_ == server.protocol) {
		int masterSocket = sock(SOCK_DGRAM, IPPROTO_UDP);
		if (masterSocket < 0) return -1;

		socklen_t clilen;
		struct sockaddr_in cliaddr;

		std::cout << "Waiting client.\n";	
	
		while(true) {
		
			do {
	
				clilen = sizeof(cliaddr);

				if ((msg_size = recvfrom(masterSocket, &buf, sizeof(buf), MSG_NOSIGNAL, (struct sockaddr*) &cliaddr, &clilen)) < 0 ) {
					std::cout << "Receive from failed!\n";
					return -1;
				}

				process_and_print(buf);
					
				if (sendto(masterSocket, &buf, sizeof(buf), MSG_NOSIGNAL, (struct sockaddr*) &cliaddr, clilen) < 0) {
					std::cout << "SendTo failed\n";
					return -1;
				}
			} while (msg_size > 0);					

		}
		close(masterSocket);
		return 0;
	}


	return 1;
}
