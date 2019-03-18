//
// Created by Edo on 2/11/2019.
//
#include <thread>
#include <string.h>
#include "vusocket.h"
#include "Client.h"
#include <iostream>

using namespace std;

void Client::createSocketAndLogIn() {
    OutputDebugStringW(L"Creating socket.");
    std::cout << "Creating socket and log in" << std::endl;

    loginStatus = ConnStatus::IN_PROGRESS;
    struct addrinfo hints = {0}, *addrs;

    const char *host = "localhost";
    const char *port = "27015";

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    sock_init();

    const int addrinfo = getaddrinfo(host, port, &hints, &addrs);

    if (addrinfo != 0) {
        std::cout << "Host not found." << std::endl;
        abort();
    }

    for (struct addrinfo *adr = addrs; adr != nullptr; adr = adr -> ai_next) {
        if ((sock = socket(adr -> ai_family, adr -> ai_socktype, adr -> ai_protocol)) == -1) {
            sock_error_code();
            continue;
        }
        if (connect((SOCKET) sock, adr -> ai_addr, adr -> ai_addrlen) == -1) {
            sock_error_code();
            sock_close(sock);
            continue;
        }
        std::cout << "Successfully connected to the server" << std::endl;
        break;
    }

    while (loginStatus == ConnStatus::IN_PROGRESS) {
        if (send_username()) {
            loginStatus = receive_response_from_server();
        }
    }
}

int Client::check_user_command(string message) {
    char buffer[BUFFER_LENGTH];
    strcpy(buffer, message.c_str());

    if (message[strlen(buffer) - 1] != '\n')
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
    else if (!strncmp(buffer, "NEXT-TURN", 9))
        return 11;
    else if (!strncmp(buffer, "NEXT-TURN", 9))
        return 11;
    else
        return -1;
}

int Client::check_server_command(string message){
    char buffer[BUFFER_LENGTH];
    strcpy(buffer, message.c_str());

    if (message[strlen(buffer) - 1] != '\n')
        return 0;

    if (!strncmp(buffer, "HELLO", 10))
        return 1;
    else if (!strncmp(buffer, "LIST", 4))
        return 2;
    else if (!strncmp(buffer, "JOINED LOBBY", 5))
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
    else if (!strncmp(buffer, "NEXT-TURN", 9))
        return 11;
    else if (!strncmp(buffer, "NEXT-TURN", 9))
        return 11;
    else
        return -1;
}

//TODO Move the actions in main to here
int Client::command_lobby(string message) {
    int action = check_user_command(message);

    switch (action) {
        case 1: {
            break;
        }
        case 2: {
            break;
        }
        case 5: {

        }
        case 9: {
            ingame = 1;
        }
    }
}

int Client::command_ingame(string message) {
    int action = check_server_command(message);

    switch (action) {
        case 0: {
            break;
        }
        case 4: {
            int x = message[5] + '0';
            int y = message[7] + '0';
            update_field(x, y, 's');
            break;
        }
        case 5: {
            int x = message[5] + '0';
            int y = message[7] + '0';
            update_field(x, y, 's');
            break;
        }
        case 7: {
            int id = message[5] + '0';
            int x = message[7] + '0';
            int y = message[9] + '0';
            update_field(x, y, 'p', id);
            break;
        }
        case 9: {
            set_field();
            break;
        }
    }
}

int Client::tick() {
    if(loginStatus == ConnStatus::SUCCESS){
        if(stdinBuffer.hasLine()){
            std::string to_server;
            to_server = stdinBuffer.readLine();

            int user_command = check_user_command(to_server);

            if (user_command > 0)
                send(sock, to_server.c_str(), strlen(to_server.c_str()), 0);
        }

        if(socketBuffer.hasLine()){
            string from_server;
            from_server = socketBuffer.readLine();

            if (ingame < 1) {
                command_lobby(from_server);
            } else {
                command_ingame(from_server);
            }

            std::cout << "SERVER: " << from_server;
        }
        return 0;
    }
    return -1;
}

bool Client::quit(char msg[]) {
    return strcmp(msg, "!exit\n") == 0;
}

int Client::readFromSocket() {
    memset(&message.in, 0x00, sizeof(message.in));

    int length = recv(sock, message.in, 1024, 0);

    if(length != -1) {
        string output;
        output = message.in;
        char msg[length];
        strcpy(msg, output.c_str());
        msg[length] = '\0';
        socketBuffer.writeChars(msg, length);
        return length;
    }
    return length;
}

int Client::readFromStdin() {
    char input[4096];

    std::cin.getline(input,sizeof(input));

    int length = strlen(input) + 1;
    char msg[length];
    strcpy(msg, input);
    msg[length - 1] = '\n';
    msg[length] = '\0';

    if(quit(msg)){
        std::cout << "Exiting client" << std::endl;
        loginStatus = ConnStatus::QUIT;
        return -1;
    }

    stdinBuffer.writeChars(msg, length);
    return length;
}

// Send username to server
bool Client::send_username() {
    memset(&message.out, 0x00, sizeof(message.out));

    char username[] = "", send_message[] = "HELLO-FROM ";
    std::cout << "Please enter your user name: ";

    fgets(message.out, BUFFER_LENGTH, stdin);
    strcat(username, message.out);

    if (quit(username)){
        loginStatus = ConnStatus::QUIT;
        std::cout << "Quiting chat client." << std::endl;
        return false;
    }

    strcat(send_message, username);
    puts(send_message);

    int send_len = send(sock, send_message, strlen(send_message), 0);

    if (send_len) {
        OutputDebugStringW(L"Send Success! SIZE: " + send_len);
        return true;
    }else {
        OutputDebugStringW(L"Error sending username.");
        return false;
    }
}

// Receive response of server
ConnStatus Client::receive_response_from_server() {
    memset(&message.in, 0x00, sizeof(message.in));

    int recv_length = recv(sock, message.in, 1024, 0);

    if (recv_length != -1) {
        OutputDebugStringW(L"Send Success! SIZE: " + recv_length);
        std::cout << "SERVER: " << message.in << std::endl;

        if (!strncmp("IN-USE", message.in, 6)) {
            OutputDebugStringW(L"Username in-use, ask for new username.");
            printf("Username in-use, try another username.");
            return ConnStatus::IN_PROGRESS;
        }
        else if (!strncmp("BUSY", message.in, 4)) {
            std::cout << ("Server is busy. Try again later.") << std::endl;
            return ConnStatus::BUSY;
        }
        else {
            OutputDebugStringW(L"Successfully established connection.");
            return ConnStatus::SUCCESS;
        }
    }else {
        std::cout << "Failed to establish connection with server";
        return ConnStatus::FAILED;
    }
}

void Client::closeSocket() {
    sock_close(sock);
    sock_quit();
    std::cout << "Closing socket" << std::endl;
}

void Client::set_field() {
    for (int i = 1; i < MAX_x + 1; ++i) {
        enemyField[i][0] = (char) i;
    }
    for (int j = 1; j < MAX_y + 1; ++j) {
        enemyField[0][j] = (char) j;
    }

    for (int k = 1; k < MAX_x; ++k) {
        for (int i = 1; i < MAX_y; ++i) {
            enemyField[k][i] = '.';
        }
    }

    for (int i = 1; i < MAX_x + 1; ++i) {
        ownField[i][0] = (char) i;
    }
    for (int j = 1; j < MAX_y + 1; ++j) {
        ownField[0][j] = (char) j;
    }

    for (int k = 1; k < MAX_x; ++k) {
        for (int i = 1; i < MAX_y; ++i) {
            ownField[k][i] = '.';
        }
    }

    for (int k = 0; k < MAX_x + 1; ++k) {
        for (int i = 0; i < MAX_y + 1; ++i) {
            cout << enemyField[k][i];
        }
        cout << endl;
    }

    for (int k = 0; k < MAX_x + 1; ++k) {
        for (int i = 0; i < MAX_y + 1; ++i) {
            cout << ownField[k][i];
        }
        cout << endl;
    }
}

void Client::update_field(int x, int y, char action, int id_s) {
    if (action == 'h') {
        enemyField[x][y] = 'X';

        for (int k = 0; k < MAX_x + 1; ++k) {
            for (int i = 0; i < MAX_y + 1; ++i) {
                cout << enemyField[k][i];
            }
            cout << endl;
        }
    } else if (action == 's') {
        copy(enemyField[0], enemyField[MAX_x + 1], scanField[0]);
        for (int i = -2; i < 3; ++i) {
            for (int j = -2; j < 3; ++j) {
                scanField[i + x][j + y] = '?';
            }
        }

        for (int k = 0; k < MAX_x + 1; ++k) {
            for (int i = 0; i < MAX_y + 1; ++i) {
                cout << scanField[k][i];
            }
            cout << endl;
        }
    } else if (action == 'm') {
        
    } else if (action == 'p') {
        ships[id_s].x = x;
        ships[id_s].y = y;
        ownField[x][y] = 'H';
        for (int k = 0; k < MAX_x + 1; ++k) {
            for (int i = 0; i < MAX_y + 1; ++i) {
                cout << enemyField[k][i];
            }
            cout << endl;
        }

    }

    for (int k = 0; k < MAX_x + 1; ++k) {
        for (int i = 0; i < MAX_y + 1; ++i) {
            cout << ownField[k][i];
        }
        cout << endl;
    }
}

