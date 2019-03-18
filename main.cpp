#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>
using namespace std;

int main() {
    bool list_action = false;
    bool lobby_action = false;
    bool attacking_action = false;
    bool scan_action = false;
    bool move_action = false;
    bool place_action = false;
    bool leave_action = false;
    string action_input;
    int x, y;
    int lobby_number;


    //FIX ME    Wait and receive a message every little while until a message is received stating that it is your turn.
    bool player_turn = true;


    //ACTION CHOICE
    while (player_turn == true) {
        cout << "Action please: " << endl;
        cin >> action_input;

        if (action_input == "LIST") {
            list_action = true;
            cout << "List test 1";
            player_turn = false;
        }
        else if (action_input == "LOBBY") {
            lobby_action = true;
            cout << "lobby test ";
            player_turn = false;
        }
        else if (action_input == "ATTACK") {
            attacking_action = true;
            player_turn = false;
        }
        else if (action_input == "SCAN") {
            scan_action = true;
            player_turn = false;
        }
        else if (action_input == "MOVE") {
            move_action = true;
            player_turn = false;
        }
        else if (action_input == "PLACE") {
            place_action = true;
            player_turn = false;
        }
        else if (action_input == "LEAVE") {
            leave_action = true;
            player_turn = false;
        }
        else {
            cout << "You have entered an invalid action, please try again." << endl;
        }
    }


    //LIST
    while(list_action == true) {
        cout << "Retrieving the list of players in the current session..." << endl;
        //FIX ME    Wait a short while, retrieve a message back with the user list, copy it to a string and print it to the client

        list_action = false;
    }


    //LOBBY
    while(lobby_action == true) {
        //...
    }


    //ATTACK
    while (attacking_action == 1) {
        cout << "What coordinates would you like to target?" << endl;
        cin >> x >> y;      //FIX ME    possibly add in a try-catch block, inputting e.g. letters will cause the program to get stuck in an infinite loop

        if ((x >= 0) && (x < 12) && (y >= 0) && (y <= 12)) {
            cout << "These are correct coordinates!" << endl;
            cout << "You chose to attack <" << x << ", " << y << ">" << endl;
            //FIX ME    Send_message "ATTACK x y" to server, server will pick [6] in the string as x, and [8] as y
            //FIX ME    Wait a short while, receive message back stating whether it was a success, copy the message to a string and print it to the client
            attacking_action = false;
        }
        else if ((x < 0) || (x > 12)) {
            cout << "You have given an incorrect X coordinate, try again. (Use the format \"x y\")" << endl;

        }
        else if ((y < 0) || (y > 12)) {
            cout << "You have given an incorrect Y coordinate, try again. (Use the format \"x y\")" << endl;
        }

    }


    //SCAN
    while(scan_action == true) {
        cout << "Scanning the board for hostile ships..." << endl;
        //FIX ME    Send_message "SCAN" instructing the server to scan the board
        //FIX ME    receive a message with coordinates of a ship, copy it to a string and print it to the client
        scan_action = false;
    }


    //MOVE
    while(move_action == true) {
        //...
        move_action = false;
    }


    //PLACE
    while(place_action == true) {
        //...
        place_action = false;
    }


    //LEAVE
    while(leave_action == true) {
        //Disconnect from the socket
        cout << "You have successfully disconnected from the server." << endl;
        leave_action = false;
    }


    if (player_turn == false) {
        //FIX ME    wait and receive a message every little while until a message is received stating that it is your turn
        //FIX ME    once that specific message is received, set player_turn back to true to get into the loops again for an action
    }




    return 0;
}


//