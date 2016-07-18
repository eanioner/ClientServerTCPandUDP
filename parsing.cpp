#include <getopt.h>
#include <iostream>
#include <stdlib.h>

#include "parsing.h"
#include "constants.h"

bool set_settings(int argc, char **argv, char *protocol, char **host, int *port) {
   
	struct option opts[] = {
		{"host", required_argument, 0, 'h'},
		{"udp", no_argument, 0, 'u'},
		{"tcp", no_argument, 0, 't'},
		{"port", required_argument, 0, 'p'},
		{0,0,0,0}
	};

	int host_counter(0), port_counter(0), tcp_counter(0), udp_counter(0);
	
	int opchar = 0;
	int opindex = 0;
	while (-1 != (opchar = getopt_long(argc, argv, "tuh:p:", opts, &opindex))) {
		switch (opchar) {
			case 't':
				if (udp_counter > 0) {
					std::cout << "Error! You cannot use both UDP and TCP at the same time. Please, try again!\n";
					return false;
				}
				if (tcp_counter > 0) {
					std::cout << "Error! You have used parameter --tcp or -t twice. Please, try again!\n";
					return false;
				}
				tcp_counter++;
				*protocol = _TCP_;
				break;
			case 'u':
				if (tcp_counter > 0) {
					std::cout << "Error! You cannot use both UDP and TCP at the same time. Please, try again!\n";
					return false;
				}
				if (udp_counter > 0) {
					std::cout << "Error! You have used parameter --udp or -u twice. Please, try again!\n";
					return false;
				}
				udp_counter++;
				*protocol = _UDP_;
				break;
			case 'h':
				if (host_counter > 0) {
					std::cout << "Error! You have used parameter --host(-h) twice. Please, try again!\n";
					return false;
				}
				host_counter++;
				*host = (char *) optarg;
				break;
			case 'p':
				if (port_counter > 0) {
					std::cout << "Error! You have used parameter --port(-p) twice. Please, try again!\n";
					return false;
				}
				port_counter++;
				*port = atoi(optarg);	// warning, it may cause errors....
				break;
			default:
				std::cout << "Error " << (char)opchar << std::endl;
				return false;
				break;
		}
	}

	

	return true;
 }

