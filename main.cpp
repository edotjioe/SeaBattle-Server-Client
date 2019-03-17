#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

#define IP_ADDRESS "127.0.0.1"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#define DEFAULT_LEN_HELLO 11
#define MAX_CLIENTS 8
#define MAX_LOBBY 4
#define MAX_SHIPS 3
#define MAX_PLAYERPLOBBY 2

using namespace std;

struct client_type
{
    int id;
    string name;
    SOCKET socket;
    int inGame;
};

struct send_message
{
    string name;
    string message;
};

struct ship {
    int x;
    int y;
    int r = 0;
    int a = 1;
};

struct player {
    int id;
    ship ships[MAX_SHIPS];
    int turn = 0;
};

struct lobby {
    int id = 0;
    player players[MAX_PLAYERPLOBBY];
    int started = 0;
};

const char OPTION_VALUE = 1;
vector<client_type> client(MAX_CLIENTS);
vector<lobby> list_lobby(MAX_LOBBY);

//Function Prototypes
int process_client(client_type &new_client, vector<client_type> &client_array, thread &thread);
int main();
int command(string message, client_type &send_client, vector<client_type> &client_array);
int check_user_command(string message);

send_message construct_message(string message);

send_message construct_message(string message) {
    int end_name, failed = 0;
    send_message sendMessage;
    sendMessage.name = "";
    sendMessage.message = "";
    for (int i = 5; i < message.size(); ++i) {
        if (message[i] == ' ') {
            end_name = i;
            break;
        } else if (message[i] == '\n')
            failed = 1;
        else
            sendMessage.name += message[i];
    }

    if (!failed)
        sendMessage.message = string(&message[end_name], &message[message.size()]);

    return sendMessage;
}

int check_user_command(string message) {
    char buffer[DEFAULT_BUFLEN];
    strcpy(buffer, message.c_str());

    if (message[strlen(message.c_str()) - 1] != '\n')
        return 0;

    if (!strncmp(buffer, "HELLO-FROM", 10))
        return 1;
    else if (!strncmp(buffer, "LIST", 4))
        return 2;
    else if (!strncmp(buffer, "LOBBY", 5))
        return 3;
    else if (!strncmp(buffer, "ATTACK", 6))
        return 4;
    else if (!strncmp(buffer, "SCAN", 4))
        return 5;
    else if (!strncmp(buffer, "MOVE", 4))
        return 6;
    else if (!strncmp(buffer, "PLACE", 5))
        return 7;
    else if (!strncmp(buffer, "LEAVE", 5))
        return 8;
    else if (!strncmp(buffer, "START", 5))
        return 9;
    else if (!strncmp(buffer, "DELIVERY", 8))
        return 10;
    else
        return 0;
}

int command(string message, client_type &send_client, vector<client_type> &client_array) {
    string output;

    int player = list_lobby[send_client.inGame].players[0].id == send_client.id;
    int player2 = (player + 1) % 2;

    if (list_lobby[send_client.inGame].players[player].turn > 0)
    switch (check_user_command(message)) {
        case 0: {
            output = "BAD-RQST\n";
            if (send(send_client.socket, output.c_str(), strlen(output.c_str()), 0) > 0)
                return 1;
            else
                return 0;
        }
        case 2: {
            output = "LIST ";
            for (int i = 0; i < list_lobby.size(); ++i) {
                if (list_lobby[i].players[1].id >= 0 || list_lobby[i].players[2].id >= 0) {
                    output += i;
                    output += " ";
                }
            }
            send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
            return 1;
        }
        case 3: {
            int index = (int) message[6];
            send_client.inGame = index;
            for (int i = 0; i < MAX_PLAYERPLOBBY; ++i) {
                if (list_lobby[index].players[i].id < 0) {
                    list_lobby[index].players[i].id = send_client.id;
                    output = "JOINED LOBBY " + list_lobby[index].id + '\n';
                    send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
                    return 1;
                }
            }
            output = "LOBBY " + index;
            output += " IS FULL\n";
            send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
            return 1;
        }
        //ATTACK
        case 4: {
            if (list_lobby[send_client.inGame].started < 2) {
                output = "GAME NOT YET STARTED\n";
                send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
                return 1;
            }

            int x = (int) message[7];
            int y = (int) message[9];
            int hit = 0;

            for (int i = 0; i < MAX_SHIPS; ++i) {
                if (list_lobby[send_client.inGame].players[player2].ships[i].x == x &&
                    list_lobby[send_client.inGame].players[player2].ships[i].y == y)
                    hit = 1;
            }

            int countDestroyed = 0;
            for (int j = 0; j < MAX_SHIPS; ++j) {
                if (list_lobby[send_client.inGame].players[player2].ships[j].a < 1)
                    countDestroyed++;
            }

            if (countDestroyed >= 3) {
                output = "YOU WIN!\n";
                send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
                output = "YOU LOST\n";
                send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                     output.c_str(), strlen(output.c_str()), 0);
            }

            if (hit) {
                output = "PLAYER " + client_array[list_lobby[send_client.inGame].players[player2].id].name + " HIT\n";
            } else {
                output = "PLAYER " + client_array[list_lobby[send_client.inGame].players[player2].id].name + " MISSED\n";
            }
            send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
            send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                    output.c_str(), strlen(output.c_str()), 0);
            return 1;
        }
        //SCAN
        case 5: {
            if (list_lobby[send_client.inGame].started < 2) {
                output = "GAME NOT YET STARTED\n";
                send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
                return 1;
            }

            int ship;

            while (true) {
                ship = rand() % 3 + 1;
                if (list_lobby[send_client.inGame].players[player2].ships[ship].a > 0)
                    break;
            }

            int x = list_lobby[send_client.inGame].players[player2].ships[ship].x;
            int y = list_lobby[send_client.inGame].players[player2].ships[ship].y;

            x += rand() % 4 + (- 2);
            y += rand() % 4 + (- 2);

            output = "SCAN ";
            output += x + " " + y + '\n';
            send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
        }
        //MOVE
        case 6: {
            if (list_lobby[send_client.inGame].started < 2) {
                output = "GAME NOT YET STARTED\n";
                send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
                return 1;
            }
            int ship = (int) message[5];
            char action = (char) message[7];

            if (action == 'r') {
                list_lobby[send_client.inGame].players[player].ships[ship].r =
                        list_lobby[send_client.inGame].players[player].ships[ship].r + 1 % 2;
            } else if (action == 'm') {
                int direction = (int) message[9];
                if (direction == 0)
                    list_lobby[send_client.inGame].players[player].ships[ship].x--;
                if (direction == 1)
                    list_lobby[send_client.inGame].players[player].ships[ship].x++;
                if (direction == 2)
                    list_lobby[send_client.inGame].players[player].ships[ship].y--;
                if (direction == 3)
                    list_lobby[send_client.inGame].players[player].ships[ship].y++;

                output = "MOVED SHIP " + ship + '\n';
                send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
                return 1;
            } else {
                output = "BAD FORMAT, TRY AGAIN. MOVE <ship id> <r/m> <1:4>\n";
                send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
                return 1;
            }
        }
        //PLACE
        case 7: {
            int ship = (int) message[6];
            int x = (int) message[8];
            int y = (int) message[10];

            list_lobby[send_client.inGame].players[player].ships[ship].x = x;
            list_lobby[send_client.inGame].players[player].ships[ship].y = y;

            output = "PLACED SHIP " + ship + '\n';
            send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
            return 1;
        }
        //LEAVE
        case 8: {
            output = "GAME ENDED: PLAYER " + send_client.name + " LEFT THE GAME\n";
            send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                 output.c_str(), strlen(output.c_str()), 0);

            output = "GAME ENDED";
            send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
        }
        //START
        case 9: {
            list_lobby[send_client.inGame].started++;

            if (list_lobby[send_client.inGame].started >= 2) {
                output = "GAME HAS STARTED!\n";
                send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
                send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                     output.c_str(), strlen(output.c_str()), 0);
                return 1;
            } else if (list_lobby[send_client.inGame].started < 2) {
                output = "VOTED TO START, WAITING ON OTHER PLAYER\n";
                send(send_client.socket, output.c_str(), strlen(output.c_str()), 0);
                output = "OTHER PLAYER IS WAITING ON YOU.\n";
                send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                     output.c_str(), strlen(output.c_str()), 0);
                return 1;
            }
        }
        //DELIVERY
        case 10: {

        }
    }

    return 1;

//    if (message[message.size() - 1] != '\n') {
//        message = "BAD-RQST-BDY\n";
//        if (send(send_client.socket, message.c_str(), strlen(message.c_str()), 0) > 0)
//            return 1;
//        else
//            return 0;
//    } else if (message.compare("WHO\n") == 0) {
//        cout << "Asked who" << endl;
//        message = "WHO-OK ";
//        for (int i = 0; i < MAX_CLIENTS; ++i) {
//            if (client[i].name.compare("") != 0) {
//                if (i != 0)
//                    message += ", ";
//                message += client[i].name;
//            }
//        }
//        message += '\n';
//
//        if (send(send_client.socket, message.c_str(), strlen(message.c_str()), 0) > 0)
//            return 1;
//        else
//            return 0;
//    } else if (strcmp(string(&message[0], &message[3]).c_str(), "SEND")) {
//        send_message s_message = construct_message(message);
//
//        cout << "Session: " << s_message.name << ", " << s_message.message;
//        if (s_message.message == "") {
//            message = "BAD-RQST-BDY\n";
//            if (send(send_client.socket, message.c_str(), strlen(message.c_str()), 0) > 0)
//                return 1;
//            else
//                return 0;
//        }
//
//        for (int i = 0; i < MAX_CLIENTS; ++i) {
//            if (!strcmp(client[i].name.c_str(), s_message.name.c_str())) {
//                message = "DELIVERY ";
//                message += send_client.name;
//                message += s_message.message;
//
//                cout << "Session: " << message;
//
//                if (send(client[i].socket, message.c_str(), strlen(message.c_str()), 0) > 0){
//                    message = "SEND-OK\n";
//                    cout << "Session: " << message;
//                    send(send_client.socket, message.c_str(), strlen(message.c_str()), 0);
//                    return 1;
//                } else
//                    return 0;
//            }
//        }
//
//        message = "UNKNOWN\n";
//        if (send(send_client.socket, message.c_str(), strlen(message.c_str()), 0) > 0)
//            return 1;
//        else
//            return 0;
//    } else {
//        message = "BAD-RQST-HDR\n";
//        if (send(send_client.socket, message.c_str(), strlen(message.c_str()), 0) > 0)
//            return 1;
//        else
//            return 0;
//    }
}

int process_client(client_type &new_client, vector<client_type> &client_array, thread &thread)
{
    string msg = "";
    char tempmsg[DEFAULT_BUFLEN] = "";

    //Handshake
    while (1)
    {
        memset(tempmsg, 0, DEFAULT_BUFLEN);

        if (new_client.socket != 0)
        {
            int iResult = recv(new_client.socket, tempmsg, DEFAULT_BUFLEN, 0);
            cout << "Handshake: " << tempmsg;
            if (iResult != SOCKET_ERROR)
            {
                string part;
                part = string(&tempmsg[DEFAULT_LEN_HELLO], &tempmsg[strlen(tempmsg)]);

                bool unique_name;
                for (int i = 0; i < client_array.size(); ++i) {
                    if (new_client.id != i) {
                        unique_name = client_array[i].name.compare(string(&part.c_str()[0],
                                                                          &part.c_str()[strlen(part.c_str()) - 1])) ? 1: 0;
                        break;
                    }
                }

                if (unique_name) {
                    msg = "HELLO ";
                    msg += part;
                    cout << "Handshake: " << msg;
                    send(new_client.socket, msg.c_str(), strlen(msg.c_str()), 0);

                    client_array[new_client.id].name = string(&part.c_str()[0],
                                                              &part.c_str()[strlen(part.c_str()) - 1]);

                    break;
                } else {
                    cout << send(new_client.socket, "IN-USE\n", 8, 0);
                }

            }
            else
            {
                msg = "Client #" + std::to_string(new_client.id) + " Disconnected";

                std::cout << msg << std::endl;

                closesocket(new_client.socket);
                closesocket(client_array[new_client.id].socket);
                client_array[new_client.id].socket = INVALID_SOCKET;

                break;
            }
        }
    } //end while

    //Session
    while (1)
    {
        memset(tempmsg, 0, DEFAULT_BUFLEN);

        if (new_client.socket != 0)
        {
            int iResult = recv(new_client.socket, tempmsg, DEFAULT_BUFLEN, 0);
            cout << "Session: " << tempmsg;
            if (iResult != SOCKET_ERROR)
            {
                command(tempmsg, new_client, client_array);
            }
            else
            {
                msg = "Client #" + std::to_string(new_client.id) + " Disconnected";

                std::cout << msg << std::endl;

                closesocket(new_client.socket);
                closesocket(client_array[new_client.id].socket);
                client_array[new_client.id].socket = INVALID_SOCKET;

                break;
            }
        }
    } //end while

    thread.detach();

    return 0;
}

int main()
{
    WSADATA wsaData;
    struct addrinfo hints;
    struct addrinfo *server = NULL;
    SOCKET server_socket = INVALID_SOCKET;
    std::string msg = "";
    int num_clients = 0;
    int temp_id = -1;
    std::thread my_thread[MAX_CLIENTS];

    //Initialize Winsock
    std::cout << "Intializing Winsock..." << std::endl;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //Setup hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Setup Server
    std::cout << "Setting up server..." << std::endl;
    getaddrinfo(static_cast<LPCTSTR>(IP_ADDRESS), DEFAULT_PORT, &hints, &server);

    //Create a listening socket for connecting to server
    std::cout << "Creating server socket..." << std::endl;
    server_socket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

    //Setup socket options
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &OPTION_VALUE, sizeof(int)); //Make it possible to re-bind to a port that was used within the last 2 minutes
    setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, &OPTION_VALUE, sizeof(int)); //Used for interactive programs

    //Assign an address to the server socket.
    std::cout << "Binding socket..." << std::endl;
    bind(server_socket, server->ai_addr, (int)server->ai_addrlen);

    //Listen for incoming connections.
    std::cout << "Listening..." << std::endl;
    listen(server_socket, SOMAXCONN);

    //Initialize the client list
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client[i] = { -1, "", INVALID_SOCKET };
    }

    while (1)
    {
        SOCKET incoming = INVALID_SOCKET;
        incoming = accept(server_socket, NULL, NULL);

        if (incoming == INVALID_SOCKET) continue;

        //Reset the number of clients
        num_clients = -1;

        //Create a temporary id for the next client
        temp_id = -1;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {

            if (client[i].socket == INVALID_SOCKET && temp_id == -1)
            {
                client[i].socket = incoming;
                client[i].id = i;
                temp_id = i;
            }

            if (client[i].socket != INVALID_SOCKET)
                num_clients++;
        }

        if (temp_id != -1)
        {
            //Create a thread process for that client
            my_thread[temp_id] = std::thread(process_client, std::ref(client[temp_id]), std::ref(client), std::ref(my_thread[temp_id]));
        }
        else
        {
            msg = "BUSY\n";
            send(incoming, msg.c_str(), strlen(msg.c_str()), 0);
            std::cout << msg << std::endl;
        }
    } //end while


    //Close listening socket
    closesocket(server_socket);

    //Close client socket
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        my_thread[i].detach();
        closesocket(client[i].socket);
    }

    //Clean up Winsock
    WSACleanup();
    std::cout << "Program has ended successfully" << std::endl;

    system("pause");
    return 0;
}