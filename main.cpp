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
#define MAX_x 12
#define MIN_x 1
#define MAX_y 12
#define MIN_y 1

using namespace std;

struct client_type
{
    int id = 0;
    string name;
    SOCKET socket;
    int inGame = -1;
};

struct send_message
{
    string name;
    string message;
};

struct ship {
    int x = -1;
    int y = -1;
    int r = 0;
    int a = 1;
};

struct player {
    int id = -1;
    ship ships[MAX_SHIPS];
    int turn = 1;
};

struct lobby {
    player players[MAX_PLAYERPLOBBY];
    int voted[MAX_PLAYERPLOBBY] = {0, 0};
    int stage = 0;
};

const char OPTION_VALUE = 1;
vector<client_type> client(MAX_CLIENTS);
vector<lobby> list_lobby(MAX_LOBBY);

//Function Prototypes
int process_client(client_type &new_client, vector<client_type> &client_array, thread &thread);
int main();
int command_lobby(string message, client_type &send_client, vector<client_type> &client_array);
int command_ingame(string message, client_type &send_client, vector<client_type> &client_array);
int check_user_command(string message);
int check_crd(string message);

int check_crd(string message){
    int result;
    string buffer;
    if (message[1] == 0x20) {
        char str = message[0];
        result = (int) str - '0';
    } else {
        buffer = message[0];
        buffer += message[1];
        result = atoi(buffer.c_str());
    }
    return result;
}

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
        return -1;
}

int command_lobby(string message, client_type &send_client, vector<client_type> &client_array) {
    string output_str;
    cout << "USER-LOBBY: " << message;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int player2 = list_lobby[send_client.inGame].players[0].id == send_client.id;
    int player = (player2 + 1) % 2;

    int user_command = check_user_command(message);

    switch (user_command) {
        case 0: {
            output_str = "BAD-RQST\n";
            if (send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0) > 0)
                return 1;
            else
                return 0;
        }
        //LIST lobby
        case 2: {
            output_str = "LIST";
            for (int h = 0; h < MAX_LOBBY; h++) {
                if (list_lobby[h].players[0].id >= 0 || list_lobby[h].players[1].id >= 0) {
                    output_str += " ";
                    output_str += '0' + h;
                }
            }
            output_str += '\n';
            cout << output_str << endl;
            send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
            return 1;
        }
        //JOIN lobby
        case 3: {
            if (send_client.inGame < 0) {
                char str = message[6];
                int index = str - '0';
                send_client.inGame = index;
                for (int i = 0; i < MAX_PLAYERPLOBBY; ++i) {
                    if (list_lobby[index].players[i].id < 0) {
                        list_lobby[index].players[i].id = send_client.id;
                        output_str = "JOINED LOBBY ";
                        output_str += '0' + index;
                        output_str += '\n';
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        output_str = send_client.name.c_str();
                        output_str += " JOINED THE LOBBY.\n";
                        if (list_lobby[send_client.inGame].players[player2].id >= 0)
                            send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                                 output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }
                }
                output_str = "LOBBY ";
                output_str += '0' + index;
                output_str += " IS FULL\n";
                send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                return 1;
            }
        }
        //LEAVE
        case 8: {
            if (send_client.inGame >= 0) {
                if (list_lobby[send_client.inGame].players[player2].id >= 0) {
                    output_str = send_client.name.c_str();
                    output_str += " LEFT THE LOBBY.\n";
                    send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                         output_str.c_str(), strlen(output_str.c_str()), 0);
                }
                output_str = "LOBBY LEFT.\n";
                send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                list_lobby[send_client.inGame].players[player].id = -1;
                list_lobby[send_client.inGame].voted[player] = 0;
                send_client.inGame = -1;
                return 1;
            }
        }
        //START
        case 9: {
            if (send_client.inGame >= 0) {
                if (list_lobby[send_client.inGame].voted[0] < 1 || list_lobby[send_client.inGame].voted[1] < 1)
                    list_lobby[send_client.inGame].voted[player] = 1;

                if (list_lobby[send_client.inGame].voted[0] > 0 && list_lobby[send_client.inGame].voted[1] > 0) {
                    output_str = "GAME HAS STARTED!\n";
                    send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                    send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                         output_str.c_str(), strlen(output_str.c_str()), 0);
                    list_lobby[send_client.inGame].voted[0] = 0;
                    list_lobby[send_client.inGame].voted[1] = 0;
                    list_lobby[send_client.inGame].stage = 1;
                    return 1;
                } else {
                    output_str = "VOTED TO START, WAITING ON OTHER PLAYER\n";
                    send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                    output_str = "OTHER PLAYER IS WAITING FOR YOU.\n";
                    send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                         output_str.c_str(), strlen(output_str.c_str()), 0);
                    return 1;
                }
            }
        }
        //DELIVERY
        case 10: {

        }
    }
}

int command_ingame(string message, client_type &send_client, vector<client_type> &client_array) {
    string output_str;
    cout << "USER-INGAME: " << message;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    int player2 = list_lobby[send_client.inGame].players[0].id == send_client.id;
    int player = (player2 + 1) % 2;

    int user_command = check_user_command(message);

    if (list_lobby[send_client.inGame].players[player].turn > 0) {
        switch (user_command) {
            case 0: {
                output_str = "BAD-RQST\n";
                if (send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0) > 0)
                    return 1;
                else
                    return 0;
            }
            //ATTACK
            case 4: {
                if (list_lobby[send_client.inGame].stage == 2) {
                    int x = check_crd(message.substr(7, 8));
                    int y;
                    if (x > 9)
                        y = check_crd(message.substr(10, 11));
                    else
                        y = check_crd(message.substr(9, 10));
                    int hit = 0;

                    cout << "x " << x << " y " << y << endl;
                    if (!(MAX_x >= x >= MIN_x && MAX_y >= y >= MIN_y)) {
                        output_str ="INVALID\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }

                    int countDestroyed = 0;
                    for (int i = 0; i < MAX_SHIPS; ++i) {
                        if (list_lobby[send_client.inGame].players[player2].ships[i].x == x &&
                            list_lobby[send_client.inGame].players[player2].ships[i].y == y) {
                            list_lobby[send_client.inGame].players[player2].ships[i].a = 0;
                            hit = 1;
                        }
                    }

                    for (int j = 0; j < MAX_SHIPS; ++j) {
                        if (list_lobby[send_client.inGame].players[player2].ships[j].a < 1)
                            countDestroyed++;
                    }

                    if (hit) {
                        output_str = "PLAYER ";
                        output_str += client_array[list_lobby[send_client.inGame].players[player2].id].name.c_str();
                        output_str += " HIT (";
                        output_str += x + '0';
                        output_str += ",";
                        output_str += y + '0';
                        output_str += ")\n";
                    } else {
                        output_str = "PLAYER ";
                        output_str += client_array[list_lobby[send_client.inGame].players[player2].id].name.c_str();
                        output_str += " MISSED (";
                        output_str += x + '0';
                        output_str += ",";
                        output_str += y + '0';
                        output_str += ")\n";
                    }
                    send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                    send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                         output_str.c_str(), strlen(output_str.c_str()), 0);

                    if (countDestroyed >= 3) {
                        output_str = "YOU WIN!\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        output_str = "YOU LOST\n";
                        send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                             output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }

                    list_lobby[send_client.inGame].players[player].turn = 0;
                    if (list_lobby[send_client.inGame].players[0].turn < 1 && list_lobby[send_client.inGame].players[1].turn < 1) {
                        list_lobby[send_client.inGame].players[0].turn = 1;
                        list_lobby[send_client.inGame].players[1].turn = 1;
                        output_str = "NEXT-TURN\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                             output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }
                    return 1;
                }
                cout << "GAME NOT STARTED YET" << endl;

                return 1;
            }
            //SCAN
            case 5: {
                if (list_lobby[send_client.inGame].stage == 2) {
                    int ship;

                    while (true) {
                        ship = (rand() % 3);
                        if (list_lobby[send_client.inGame].players[player2].ships[ship].a > 0)
                            break;
                    }

                    int x = list_lobby[send_client.inGame].players[player2].ships[ship].x;
                    int y = list_lobby[send_client.inGame].players[player2].ships[ship].y;

                    x = abs(x + (rand() % 5) - 2) % MAX_x;
                    y = abs(y + (rand() % 5) - 2) % MAX_y;

                    output_str = "SCAN ";
                    output_str += '0' + x;
                    output_str += " ";
                    output_str += '0' + y;
                    output_str += '\n';
                    send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);

                    cout << output_str << endl;

                    list_lobby[send_client.inGame].players[player].turn = 0;
                    if (list_lobby[send_client.inGame].players[0].turn < 1 && list_lobby[send_client.inGame].players[1].turn < 1) {
                        list_lobby[send_client.inGame].players[0].turn = 1;
                        list_lobby[send_client.inGame].players[1].turn = 1;
                        output_str = "NEXT-TURN\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                             output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }
                }
                cout << "GAME NOT STARTED" << endl;

                return 1;
            }
            //MOVE
            case 6: {
                if (list_lobby[send_client.inGame].stage == 2) {
                    int ship = message[5] - '0';
                    int direction = message[9] - '0';

                    if (list_lobby[send_client.inGame].players[player].ships[ship].a < 1) {
                        output_str = "INVALID: SHIP\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }

                    if (direction == 0 && list_lobby[send_client.inGame].players[player].ships[ship].x > MIN_x)
                        list_lobby[send_client.inGame].players[player].ships[ship].x--;
                    else if (direction == 1 && list_lobby[send_client.inGame].players[player].ships[ship].x < MAX_x)
                        list_lobby[send_client.inGame].players[player].ships[ship].x++;
                    else if (direction == 2 && list_lobby[send_client.inGame].players[player].ships[ship].y > MIN_y)
                        list_lobby[send_client.inGame].players[player].ships[ship].y--;
                    else if (direction == 3 && list_lobby[send_client.inGame].players[player].ships[ship].x < MAX_y)
                        list_lobby[send_client.inGame].players[player].ships[ship].y++;
                    else {
                        output_str = "INVALID: DIRECTION\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }

                    output_str = "MOVED SHIP ";
                    output_str += '0' + ship;
                    output_str += '\n';
                    send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                }
                list_lobby[send_client.inGame].players[player].turn = 0;
                if (list_lobby[send_client.inGame].players[0].turn < 1 &&
                    list_lobby[send_client.inGame].players[1].turn < 1) {
                    list_lobby[send_client.inGame].players[0].turn = 1;
                    list_lobby[send_client.inGame].players[1].turn = 1;
                    output_str = "NEXT-TURN\n";
                    send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                    send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                         output_str.c_str(), strlen(output_str.c_str()), 0);
                    return 1;
                }
                cout << "GAME NOT STARTED YET" << endl;

                return 1;
            }
            //PLACE
            case 7: {
                if (list_lobby[send_client.inGame].stage == 1) {
                    int ship = message[6] - '0';
                    int x = check_crd(message.substr(8, 9));
                    int y;
                    if (x > 9)
                        y = check_crd(message.substr(11, 12));
                    else
                        y = check_crd(message.substr(10, 11));

                    cout << "X: " << x << ", " << y << ", ship " << ship << endl;
                    if (MAX_SHIPS < ship < 0) {
                        output_str ="INVALID SHIP\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }

                    if (MAX_x >= x && x >= MIN_x)
                        list_lobby[send_client.inGame].players[player].ships[ship].x = x;
                    else {
                        output_str ="INVALID x\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }
                    if (MAX_y >= y >= MIN_y)
                        list_lobby[send_client.inGame].players[player].ships[ship].y = y;
                    else {
                        output_str ="INVALID y\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }

                    output_str ="VALID\n";
                    send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                }
                return 1;
            }
            //LEAVE
            case 8: {
                output_str ="GAME ENDED: PLAYER ";
                output_str += send_client.name.c_str();
                output_str += " LEFT THE GAME\n";
                send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                     output_str.c_str(), strlen(output_str.c_str()), 0);

                output_str = "GAME ENDED\n";
                send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                list_lobby[send_client.inGame].players[player].id = -1;
                list_lobby[send_client.inGame].voted[player] = 0;
                list_lobby[send_client.inGame].stage = 0;
                list_lobby[send_client.inGame].players[player].turn = 1;
                for (int i = 0; i < MAX_SHIPS; ++i) {
                    list_lobby[send_client.inGame].players[player].ships[i].x = -1;
                    list_lobby[send_client.inGame].players[player].ships[i].y = -1;
                    list_lobby[send_client.inGame].players[player].ships[i].r = 0;
                    list_lobby[send_client.inGame].players[player].ships[i].a = 1;
                }
                return 1;
            }
            //START
            case 9: {
                if (list_lobby[send_client.inGame].stage == 1) {
                    for (int i = 0; i < MAX_SHIPS; ++i) {
                        if (list_lobby[send_client.inGame].players[player].ships[i].x < 0) {
                            output_str = "PLACE SHIP ";
                            output_str += '0' + i;
                            output_str += '\n';
                            send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                            return 1;
                        }
                    }
                    list_lobby[send_client.inGame].voted[player] = 1;
                    if (list_lobby[send_client.inGame].voted[0] > 0 && list_lobby[send_client.inGame].voted[1] > 0) {
                        output_str = "GAME STARTED\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                             output_str.c_str(), strlen(output_str.c_str()), 0);
                        list_lobby[send_client.inGame].players[0].turn = 1;
                        list_lobby[send_client.inGame].players[1].turn = 1;
                        list_lobby[send_client.inGame].stage = 2;
                        return 1;
                    } else {
                        output_str = "VOTED TO START, WAITING FOR OTHER PLAYER\n";
                        send(send_client.socket, output_str.c_str(), strlen(output_str.c_str()), 0);
                        output_str = "OTHER PLAYER IS WAITING FOR YOU\n";
                        send(client_array[list_lobby[send_client.inGame].players[player2].id].socket,
                             output_str.c_str(), strlen(output_str.c_str()), 0);
                        return 1;
                    }
                }
            }
            //DELIVERY
            case 10: {

            }
        }
    }



    return 1;
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
                    new_client.inGame = -1;
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

        int player = list_lobby[new_client.inGame].players[0].id != new_client.id;

        if (new_client.socket != 0)
        {
            
            int iResult = recv(new_client.socket, tempmsg, DEFAULT_BUFLEN, 0);

            if (iResult != SOCKET_ERROR)
            {
                if (new_client.inGame < 0 || list_lobby[new_client.inGame].stage < 1)
                    command_lobby(tempmsg, new_client, client_array);
                else
                    command_ingame(tempmsg, new_client, client_array);
            }
            else
            {
                msg = "Client #" + std::to_string(new_client.id) + " Disconnected";

                string leave_message = "LEAVE\n";
                if (new_client.inGame >= 0)
                    command_ingame(leave_message, new_client, client_array);
                else
                    command_lobby(leave_message, new_client, client_array);

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