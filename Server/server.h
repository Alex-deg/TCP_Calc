#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <cerrno>
#include <stdexcept>
#include <memory>
#include <fcntl.h>
#include <stack>
#include "Calculating/Calculator.h"

class TCPServer {
public:
    TCPServer(int port);
    void run();
    ~TCPServer();
    
private:
    static const int MAX_EVENTS = 10;
    static const int BUF_SIZE = 1024;
    
    int port_;
    int epoll_fd_;
    int server_fd_;
    std::vector<struct epoll_event> events_{MAX_EVENTS};

    void createSocket();
    void setupEpoll();
    void handleNewConnection();
    void handleClientData(int fd);

    int set_nonblocking(int fd);

    std::string client_request;

};