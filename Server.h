#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

#define IP_ADDRESS "127.0.0.1"
#define MAX_CLIENTS 10
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#define DEFAULT_LEN_HELLO 11

#ifndef SEABATTLE_SERVER_SERVER_H
#define SEABATTLE_SERVER_SERVER_H

using namespace std;

struct message {
    string in;
    string out;
};

struct client_type
{
    int id;
    string name;
    SOCKET socket;
};

struct send_message
{
    string name;
    string message;
};

vector<client_type> client(MAX_CLIENTS);
const char OPTION_VALUE = 1;

class Server {
private:
    //Variables
    WSADATA wsaData;
    struct addrinfo hints;
    struct addrinfo *server = NULL;

    std::string msg = "";
    int num_clients = 0;
    int temp_id = -1;
    std::thread my_thread[MAX_CLIENTS];

    send_message construct_message(string message);
    int command(string message, client_type &send_client);
    int process_client(client_type &new_client, vector<client_type> &client_array, thread &thread);

public:
    SOCKET server_socket = INVALID_SOCKET;

    //Functions
    int initialise();
    int run();
};


#endif //SEABATTLE_SERVER_SERVER_H
