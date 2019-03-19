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
    else if (!strncmp(buffer, "LOBBY JOINED BY", 15))
        return 3;
    else if (!strncmp(buffer, "LOBBY JOINED", 12))
        return 4;
    else if (!strncmp(buffer, "LOBBY IS FULL", 13))
        return 5;
    else if (!strncmp(buffer, "LOBBY LEFT BY", 13))
        return 6;
    else if (!strncmp(buffer, "LOBBY LEFT", 10))
        return 7;
    else if (!strncmp(buffer, "STARTED", 7))
        return 8;
    else if (!strncmp(buffer, "VOTED BY OTHER PLAYER", 20))
        return 9;
    else if (!strncmp(buffer, "VOTED", 5))
        return 10;
    else if (!strncmp(buffer, "LOBBY LEFT. TIMEOUT", 19))
        return 11;
    else if (!strncmp(buffer, "PLAYER HIT", 10))
        return 12;
    else if (!strncmp(buffer, "PLAYER MISSED", 13))
        return 13;
    else if (!strncmp(buffer, "WIN", 3))
        return 14;
    else if (!strncmp(buffer, "LOST", 4))
        return 15;
    else if (!strncmp(buffer, "MOVED SHIP", 10))
        return 16;
    else if (!strncmp(buffer, "PLACED SHIP", 11))
        return 17;
    else if (!strncmp(buffer, "NEXT-TURN", 9))
        return 18;
    else if(!strncmp(buffer, "SCAN", 4))
        return 19;
    else if(!strncmp(buffer, "INVALID", 4))
        return 20;
    else if(!strncmp(buffer, "PLACE SHIP", 10))
        return 21;
    else if(!strncmp(buffer, "GAME STARTED", 12))
        return 22;
    else
        return -1;
}

//TODO Move the actions in main to here
int Client::command_lobby(string message) {
    int action = check_server_command(message);

    switch (action) {
        case 2: {
            cout << message;
            break;
        }
        case 3: {
            cout << message;
            break;
        }
        case 4: {
            cout << "You have joined a lobby, to vote to start type 'START' and to leave type 'LEAVE'\n";
            break;
        }
        case 5: {
            cout << message;
            break;
        }
        case 6: {
            cout << message;
            break;
        }
        case 7: {
            cout << message;
            break;
        }
        case 8: {
            cout << "The game has started, please place 3 ships within the field of 12x12.\n";
            cout << "A ship can be placed by typing 'PLACE <ship_id> <x> <y>'. The ship id's are from 0 to 2.\n";
            cout << "To leave the game type 'LEAVE'\n";
            set_field();
            view_field();
            ingame = 1;
            break;
        }
        case 9: {
            cout << "The other player has voted to start, please vote by typing START.\n";
            break;
        }
        case 10: {
            cout << "You voted to start the game.\n";
            break;
        }
        case 11: {
            cout << message;
            break;
        }
    }
    return 1;
}

int Client::command_ingame(string message) {
    int action = check_server_command(message);

    switch (action) {
        case 8: {
            cout << "You can now begin the war\n";
            cout << "To attack type 'ATTACK <x> <y>', where x and y are between 1 and 12.\n";
            cout << "To move type 'MOVE <ship_id> <x> <y>'.\n";
            cout << "To scan type 'SCAN'.\n";
            turn = 1;
            break;
        }
        case 9: {
            cout << message;
            break;
        }
        case 10: {
            cout << message;
            break;
        }
        case 12: { //TODO Look for fix for who hit
            int x = check_crd(message.substr(11, 12));
            int y;
            if (x > 9)
                y = check_crd(message.substr(14, 15));
            else
                y = check_crd(message.substr(13, 14));

            //update_field(x, y, 'h');
            view_field();
            cout << "You got hit!\n";
            turn = 0;
            break;
        }
        case 13: {
            cout << "You missed!\n";
            view_field();
            turn = 0;
            break;
        }
        case 14: {
            ingame = 0;
            cout << "You won the war!\n";
            break;
        }
        case 15: {
            ingame = 0;
            cout << "You lost the war!\n";
            break;
        }
        case 16: {
            int id = message[12] - '0';
            int x = check_crd(message.substr(13, 14));
            int y;
            if (x > 9)
                y = check_crd(message.substr(16, 17));
            else
                y = check_crd(message.substr(15, 16));

            update_field(x, y, 2, id);
            cout << "Moved the ship.\n";
            view_field();
            turn = 0;
            break;
        }
        case 17: {
            int id = (int) message[12] - '0';
            int x = check_crd(message.substr(14, 15));
            int y;

            ships[id].x = x;
            ships[id].y = y;

            if (x > 9)
                y = check_crd(message.substr(17, 18));
            else
                y = check_crd(message.substr(16, 17));

            update_field(x, y, 3, id);
            view_field();
            return 1;
        }
        case 18: {
            cout << message;
            turn = 1;
            break;
        }
        case 19: {
            int x = check_crd(message.substr(5, 6));
            int y;
            if (x > 9)
                y = check_crd(message.substr(8, 9));
            else
                y = check_crd(message.substr(7, 8));

            update_field(x, y, 1);
            cout << "Found a ship.\n";
            view_scan_field();
            break;
        }
        case 20: {
            cout << message;
            break;
        }
        case 21: {
            cout << message;
            break;
        }
        case 22: {
            cout << message;
        }
    }
    return 1;
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
            cout << "HAS line\n";
            string from_server;
            from_server = socketBuffer.readLine();

            if (ingame < 1) {
                int l = command_lobby(from_server);
            } else {
                int s = command_ingame(from_server);
            }

        }
        return 0;
    }
    return 1;
}

bool Client::quit(char msg[]) {
    return strcmp(msg, "!exit\n") == 0;
}

int Client::check_crd(string message){
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
    char buffer[4096];

    std::cin.getline(buffer,sizeof(buffer));

    int length = strlen(buffer) + 1;
    char msg[length];
    strcpy(msg, buffer);
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
    for (int i = 1; i < MAX_x; ++i) {
        enemyField[i][0] = to_string(i);
    }
    for (int j = 1; j < MAX_y; ++j) {
        enemyField[0][j] = to_string(j);
    }

    for (int k = 1; k < MAX_x; ++k) {
        for (int i = 1; i < MAX_y; ++i) {
            enemyField[k][i] = 'o';
        }
    }

    for (int i = 1; i < MAX_x; ++i) {
        ownField[i][0] = to_string(i);
    }
    for (int j = 1; j < MAX_y; ++j) {
        ownField[0][j] = to_string(j);
    }

    for (int k = 1; k < MAX_x; ++k) {
        for (int i = 1; i < MAX_y; ++i) {
            ownField[k][i] = 'o';
        }
    }
}

void Client::update_field(int x, int y, int action, int id_s) {
    if (action == 0) {
        enemyField[x][y] = 'X';
    } else if (action == 1) {
        copy(enemyField[0], enemyField[MAX_x], scanField[0]);
        for (int i = -2; i < 3; ++i) {
            for (int j = -2; j < 3; ++j) {
                scanField[i + x][j + y] = '?';
            }
        }
    } else if (action == 2) {
        ownField[ships[id_s].x][ships[id_s].y] = '.';
        ownField[x][y] = 'H';
    }
    else if (action == 3) {
        ships[id_s].x = x;
        ships[id_s].y = y;
        ownField[x][y] = "H";
    }
    return;
}

void Client::view_field() {
    cout << "-------Enemy field----------" << endl;
    for (int k = 0; k < MAX_x ; ++k) {
        for (int i = 0; i < MAX_y; ++i) {
            cout << enemyField[k][i] << ' ';
            if (k < 10 && i == 0)
                cout << " ";
        }
        cout << endl;
    }
    cout << "-------Your field-----------" << endl;
    for (int k = 0; k < MAX_x; ++k) {
        for (int i = 0; i < MAX_y; ++i) {
            cout << ownField[k][i] << ' ';
            if (k < 10 && i == 0)
                cout << " ";
        }
        cout << endl;
    }
    return;
}

void Client::view_scan_field() {
    cout << "----Enemy field(scanned)----" << endl;
    for (int k = 0; k < MAX_x + 1; ++k) {
        for (int i = 0; i < MAX_y + 1; ++i) {
            cout << scanField[k][i] << ' ';
            if (k < 10 && i == 0)
                cout << " ";
        }
        cout << endl;
    }
    cout << "-------Your field-----------" << endl;
    for (int k = 0; k < MAX_x + 1; ++k) {
        for (int i = 0; i < MAX_y + 1; ++i) {
            cout << ownField[k][i] << ' ';
            if (k < 10 && i == 0)
                cout << " ";
        }
        cout << endl;
    }
}