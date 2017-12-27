#ifndef UI_H
#define UI_H

#include <iostream>    //cout
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <string>  //string
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#include <sstream>
#include <unistd.h>

using namespace std;

void gui_init();
void gui_exit(int);
void gui_log(const char *s);
char *gui_input(char *s);
void gui_display(int row , int cur , int quo);

#define UIInit gui_init
#define UIExit gui_exit
#define UIDisp gui_display
#define UILog  gui_log
#define INPUT(s) fgets(s , sizeof(s), stdin)

/**
	TCP Client class
 */
class tcp_client{
	private:
		int sock;
		std::string address;
		int port;
		struct sockaddr_in server;

	public:
		tcp_client();
		bool conn(string, int);
		bool send_data(string data);
		string receive(int);
};

extern tcp_client c;
#endif
