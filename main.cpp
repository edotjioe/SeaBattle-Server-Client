#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>
using namespace std;

int main() {
    bool list_action = false, lobby_action = false, attacking_action = false, scan_action = false, move_action = false,
            place_action = false, leave_action = false, lobby_connection = false;
    string action_input;
    int x, y, ship, direction, lobby_number;
    char move_type;


    //FIX ME    Wait and receive a message every little while until a message is received stating that it is your turn.
    bool player_turn = true;


    //ACTION CHOICE
    while (player_turn == true) {
        cout << "Action please: " << endl;
        cin >> action_input;

        if (action_input == "LIST") {
            list_action = true;
            player_turn = false;
        }
        else if (action_input == "LOBBY") {
            lobby_action = true;
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
        //...
        list_action = false;
    }


    //LOBBY
    while(lobby_action == true) {
        cout << "What lobby would you like to join?" << endl;

        /*while (lobby_connection == false;) {
            cin >> lobby_number;
            if (message received == success) {
               cout << "You have successfully connected to lobby " << lobby_number << "!" << endl;
               lobby_connection = true;
        }*/
        //FIX ME    Send the lobby number to the server, wait for a response.
        //FIX ME    If the response is a success, connect to the lobby, if not, give the error message and enter a new lobby_number
        lobby_action = false;
    }


    //ATTACK
    while (attacking_action) {
        cout << "Which coordinates would you like to target?" << endl;
        cin >> x >> y;      //FIX ME    possibly add in a try-catch block, inputting e.g. letters will cause the program to get stuck in an infinite loop

        if ((x >= 0) && (x < 12) && (y >= 0) && (y <= 12)) {
            cout << "You chose to attack <" << x << ", " << y << ">!" << endl;
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
        cout << "What ship would you like to move?" << endl;
        while (1) {
            cin >> ship;
            if ((ship >= 1 ) && (ship <= 5)) {
                break;
            }
            else {
                cout << "You have given an invalid ship-number, please try again." << endl;
                cin.clear();
                cin.ignore();
            }
        }

        while (1) {
            cout << "Do you want to rotate (r) or move your ship (m)?" << endl;
            cin >> move_type;
            if ((move_type != 'r') && (move_type != 'm')) {
                cout << "You have given an invalid move type, you can rotate (r) or move (m)." << endl;
                cin.ignore();
            }
            else {
                break;
            }
        }

        if (move_type == 'm') {
            cout << "In what direction would you like to move your ship?" << endl;
            while (1) {
                cin >> direction;
                if (cin.fail()) {
                    cout << "You have given an invalid direction, please try again. \nThe options are: left '0', right '1', down '2' or up '3'." << endl;
                    cin.clear();
                    cin.ignore();
                }
                else if ((direction >= 0 ) && (direction <= 3)){
                    break;
                }
                else {
                    cout << "You have given an invalid direction, please try again. \nThe options are: left '0', right '1', down '2' or up '3'." << endl;
                    cin.clear();
                    cin.ignore();
                }
            }
        }

        else if (move_type == 'r') {
            cout << "You used rotate." << endl;
            //FIX ME
        }

        //FIX ME    Send_message "MOVE 4 1" to server, server will pick [5] in the string as ship, and [7] as the move type (r or m), if m, then [9] is the direction
        //FIX ME    This can be done by making a string consisting of "MOVE      " where [5] is changed according to ship integer,
        //FIX ME    [7] to move_type, and possibly [9] to direction
        //FIX ME    Wait a short while, receive message back stating whether it was a success, copy the message to a string and print it to the client

        move_action = false;
    }


    //PLACE
    while(place_action == true) {
        //FIX ME
        place_action = false;
    }


    //LEAVE
    while(leave_action == true) {
        //Disconnect from the socket
        cout << "You have successfully disconnected from the server." << endl;
        leave_action = false;
    }


    if (player_turn == false) {
        //FIX ME    wait and receive a message every little while until a message is received stating that it is your turn, or that the game is over
        //FIX ME    once a message is received, act accordingly, either setting the player_turn back to true, or stating that the game is over and player defeated
    }

    cout << "Test here: " << direction;
    return 0;
}

