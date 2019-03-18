//
// Created by edotj on 3/18/2019.
//

#ifndef SEABATTLE_CLIENT_CLIENT_H
#define SEABATTLE_CLIENT_CLIENT_H


#ifdef _WIN32
#else
#include <pthread.h>
#endif

#include <thread>
#include <string.h>
#include "pthread.h"
#include "vusocket.h"
#include "CircularLineBuffer.h"

#define BUFFER_LENGTH 400

using namespace std;

struct Message {
    char in[BUFFER_LENGTH];
    char out[BUFFER_LENGTH];
};

enum class ConnStatus {
    IN_PROGRESS,
    SUCCESS,
    BUSY,
    FAILED,
    QUIT
};

class Client {

private:
    struct Message message;
    ConnStatus loginStatus;

    int command_lobby(string message);
    int command_ingame(string message);
    int check_command(string message);
    bool quit(char msg[]);
    void command(char msg[]);
    bool sendUserName();
    ConnStatus receiveResponseFromServer();

    int ingame = 0;

    /**
     * You are free to add new member variables and methods here if needed.
     * Please do not remove the ones that are already here.
     */

    /**
     * You won't need to edit these variables. They're used by the code that's already given.
     */
    std::thread socketThread, stdinThread;

    /**
     * You can use sock for the socket connection with the chat server.
     */
    SOCKET sock;

    /**
     * You need these variables in assignments 3 and 4.
     * These buffers allow you to read in a character stream, and read out complete lines.
     */
    CircularLineBuffer socketBuffer, stdinBuffer;

    /**
     * Assignment 4
     *
     * This method reads data from the standard input and writes it into the 'stdinBuffer'.
     * This method is called repeatedly, which means you don't need to add a loop yourself.
     * After implementation, it should look something like this:
     *
     * std::string input = [read string from stdin];
     * stdinBuffer.writeChars([change string to char array], [number of elements]);
     *
     * Make sure that you also write newline (\n) characters into the stdinBuffer,
     * otherwise stdinBuffer.readLine() will never return a line.
     *
     * See the lab manual for more details.
     *
     * @return Returns -1 if the user writes '!exit', returns the number of characters read otherwise.
     */
    int readFromStdin();

    /**
     * Assignment 4
     *
     * This method reads data from 'sock' and writes it into 'socketBuffer'.
     * This method is called repeatedly, which means you don't need to add a loop yourself.
     * After implementation, it should look something like this:
     *
     * int numbytes = recv([socket], [buffer], [buffer size], 0);
     * socketBuffer.writeChars([buffer], numbytes);
     *
     * See the lab manual for more details.
     *
     * #return Return the return value of 'recv'.
     */
    int readFromSocket();

    /**
     * You may ignore this method.
     */
    inline void threadReadFromStdin() {
        while (!stop) {
            auto res = readFromStdin();
            if (res < 0) {
                stop = true;
            }
        }
    }

    /**
     * You may ignore this method.
     */
    inline void threadReadFromSocket() {
        while (!stop) {
            auto res = readFromSocket();
            if (res <= 0) {
                stop = true;
            }
        }
    }

    /**
     * Assignment 1
     *
     * Implement this method. It should complete the following tasks:
     * 1. Establish a socket connection to the server.
     * 2. Ask the user for a username
     * 3. Use this username to log in at the server.
     * 4. If an error occurs (for example because the username is taken), go back to step 1.
     *
     * You can find useful functions for creating and connecting using a socket in the 'vusocket.h'.
     * See the lab manual for more details on the specifics of the protocol.
     */
    void createSocketAndLogIn();

    /**
     * Assignment 1
     *
     * Implement this method. It should close socket 'sock' if it is open, and
     * clean up any other network resources that you've used.
     * Look for methods to call in 'vusocket.h'.
     *
     * Although you have to implement this method, you never need to call it yourself.
     * The code we provide takes care of that.
     */
    void closeSocket();

    /**
     * You may ignore this method.
     */
    inline void startThreads() {
        socketThread = std::thread(&Client::threadReadFromSocket, this);
        stdinThread = std::thread(&Client::threadReadFromStdin, this);
    }

    /**
     * You may ignore this method.
     */
    inline void stopThreads() {
        socketThread.join();
        stdinThread.join();
    }

public:
    /**
     * You won't need to edit this variable. It's used by the code that's already given.
     */
    bool stop = false;

    /**
     * You may ignore this method.
     */
    inline ~Client() {
        closeSocket();
        stopThreads();
    }

    /**
     * You may ignore this method.
     */
    inline void setup() {
        createSocketAndLogIn();
        startThreads();
    }

    /**
     * Assignment 2 / Assignment 4
     *
     * Implement this method. This method is called after you have successfully logged in using 'createSocketAndLogIn'
     * It is called repeatedly (see main.cpp), which means you don't need a loop in this method.
     *
     * For Assignment 2, it should perform the following tasks:
     * 1. Ask the user for input (e.g., !who, !exit, @echobot test) and take the appropriate action by sending something
     *      to the server via the socket connection, or by exiting the application.
     * 2. Receive data from the server to check that the message was successfully send (e.g., SEND-OK) and
     *      show this to the user.
     *
     * For Assignment 4, it should perform the following tasks:
     * 1. Check for user input by polling 'stdinBuffer'. If it contains a line, read it and process it.
     * 2. Check for data sent by the server by polling 'socketBuffer'. If it contains a line,
     *      read it and show it to the user.
     *
     * See the lab manual for more details.
     *
     * @return -1 if anything went wrong, or when the user types '!exit'. 0 otherwise.
     */
    int tick();
};

#endif //SEABATTLE_CLIENT_CLIENT_H
