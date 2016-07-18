#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#include "parsing.h"
#include "constants.h"

struct Client {
	char protocol;
	char *host;
	int port;
} client;


int main(int argc, char **argv) {	
	
	std::cout << "Client is running...\n";

	//default settings
	client.protocol = _TCP_;
	client.host = _LOCAL_HOST_;
	client.port = 12345;

	// get settings from command line's argument
	if (!set_settings(argc, argv, &client.protocol, &client.host, &client.port)) return -1;

 	std::cout << "Client. Protocol: " << ((_TCP_ == client.protocol) ? "TCP" : "UDP") 
		<< ", Host: " << client.host
		<< ", Port: " << client.port
		<< ".\n";
	std::cout << "Push 'Enter' twice to close client\n";

	// create and start TCP Client
	if (_TCP_ == client.protocol) {
		int masterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (masterSocket < 0) {
			std::cout << "Socket was not created!\n";
			return -1;
		}

		struct sockaddr_in sockAddr;
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(client.port);

		int ip = inet_aton(client.host, &sockAddr.sin_addr);
		if (0 == ip) {
			std::cout << "Bad host\n";
			return -1;
		}

		if (connect(masterSocket, (struct sockaddr*) &sockAddr, sizeof(sockAddr)) < 0) {
			std::cout << "Connect failed!\n";
			return -1;
		}

	
		char buf = 0;		
		while (true) {
			scanf("%c", &buf);	
			if (send(masterSocket, &buf, sizeof(buf), MSG_NOSIGNAL) < 0) {
				std::cout << "Send failed!\n";
				return -1;
			}
			buf = '\0';
			if (recv(masterSocket, &buf, sizeof(buf), MSG_NOSIGNAL) < 0) {
				std::cout << "Recieve failed!\n";
				return -1;
 			}
			
			if('\0' == buf) {
				std::cout << "Server closed connection.\n";
				break;
			} 

			printf("%c", buf);
		}
		
		close(masterSocket);
		return 0;
	}

	if (_UDP_ == client.protocol) {
		int masterSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (masterSocket < 0) {
				std::cout << "Socket was not created!\n";
				return -1;
			}

		struct sockaddr_in sockAddr;
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(client.port);

		int ip = inet_aton(client.host, &sockAddr.sin_addr);
		if (0 == ip) {
			std::cout << "Bad host\n";
			return -1;
		}

		
		struct sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(client.port);
		
		ip = inet_aton(client.host, &servAddr.sin_addr);
		if (0 == ip) {
			std::cout << "Bad host\n";
			return -1;
		}

		
		char buf = 0;

		int exit_counter = 0;
		while (true) {
			scanf("%c", &buf);

			if ('\n' == buf) {
				exit_counter++;
			} else {
				exit_counter = 0;
			}

			if (2 == exit_counter) {
				std::cout << "\nClient is closed\n";
				break;
			}

			if (sendto(masterSocket, &buf, sizeof(buf), MSG_NOSIGNAL, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
				std::cout << "Sendto failed!\n";
				return -1;
			}

			buf = '\0';
			
			if (recvfrom(masterSocket, &buf, sizeof(buf), MSG_NOSIGNAL, (struct sockaddr*) NULL, NULL) < 0) {
				std::cout << "RecieveFrom failed!\n";
				return -1;
			}			

			printf("%c", buf);
		}
		close(masterSocket);
		return 0;
	}













	return 0;
}
