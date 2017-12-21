/**
	C++ client example using sockets
 */
#include <iostream>    //cout
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <string>  //string
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#include <sstream>
#include <unistd.h>
#include "gui.h"

using namespace std;

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

tcp_client::tcp_client(){
	sock = -1;
	port = 0;
	address = "";
}

tcp_client c;
pthread_mutex_t progLock;

/**
	Connect to a host on a certain port number
 */

bool tcp_client::conn(string address , int port){
	//create socket if it is not already created
	if(sock == -1)
	{
		//Create socket
		sock = socket(AF_INET , SOCK_STREAM , 0);
		if (sock == -1)
		{
			perror("Could not create socket");
		}

		cout<<"Socket created\n";
	}
	else    {   /* OK , nothing */  }

	//setup address structure
	if(inet_addr(address.c_str()) == (in_addr_t)(-1))
	{
		struct hostent *he;
		struct in_addr **addr_list;

		//resolve the hostname, its not an ip address
		if ( (he = gethostbyname( address.c_str() ) ) == NULL)
		{
			//gethostbyname failed
			herror("gethostbyname");
			cout<<"Failed to resolve hostname\n";

			return false;
		}

		//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
		addr_list = (struct in_addr **) he->h_addr_list;

		for(int i = 0; addr_list[i] != NULL; i++)
		{
			//strcpy(ip , inet_ntoa(*addr_list[i]) );
			server.sin_addr = *addr_list[i];

			cout<<address<<" resolved to "<<inet_ntoa(*addr_list[i])<<endl;

			break;
		}
	}

	//plain ip address
	else
	{
		server.sin_addr.s_addr = inet_addr( address.c_str() );
	}

	server.sin_family = AF_INET;
	server.sin_port = htons( port );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}

	cout<<"Connected\n";
	return true;
}

/**
	Send data to the connected host
 */
bool tcp_client::send_data(string data){
	//Send some data
	if( send(sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
	{
		perror("Send failed : ");
        exit(0);
		return false;
	}

	return true;
}

/**
	Receive data from the connected host
 */
string tcp_client::receive(int size=512){
	char buffer[size];
	string reply;

	//Receive a reply from the server
	if( recv(sock , buffer , sizeof(buffer) , 0) < 0)
	{
		puts("recv failed");
	}

	reply = buffer;
	return reply;
}

string make_JSON(string key, int value){
	stringstream ss;
	ss << value;
	string str = '\"' + key + "\":" + ss.str();
	return str;
}

// { 	"bank": { "max": %d, "cur": %d },
//		"cus1": { "max": -1, "cur": %d },
//		...
//		"cus20":{ "max": -1, "cur": %d}
// }

void gui_init()
{
	string host;

	//cout<<"Enter hostname : ";
	//cin>>host;

	//connect to host
	//c.conn(host , 8001);
	c.conn("localhost" , 6969);
}
	//send some data
	//c.send_data("this is a meg!");

void gui_log(const char *s){
	cout << "log : " << s << endl;
}

void gui_display(int idx, int cur, int quo){
	pthread_mutex_lock(&progLock);
	string str = '{' + make_JSON("max", quo) + ',' +
								make_JSON("cur", cur) + '}';
	string out_data;
	stringstream ss;

    if(idx >= 0) idx += 1;
	ss << idx;
	/*if(idx == -1)
		out_data = "{\"bank\":" + str + "}";
	else
		out_data = ("{\"cus" + ss.str()) + "\":" + str + "}";*/

	out_data = "{" + ((idx == -1)? "\"bank" : ("\"cus" + ss.str())) + "\":" + str + "} ,";
	
	//cout << out_data << endl;
	c.send_data(out_data);
	pthread_mutex_unlock(&progLock);
}

void gui_exit(int v){
	exit(v);
}

