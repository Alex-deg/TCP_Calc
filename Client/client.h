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
#include <random>
#include <memory>
#include <algorithm>
#include "Calculating/Calculator.h"

class TCPClient {
public:
    TCPClient(const std::string& server_addr, int server_port, int n, int connections);
    ~TCPClient();
    void run();
private:
    static const int MAX_EVENTS = 10;
    static const int BUF_SIZE = 1024;
    
    std::string server_addr_;
    int server_port_;
    int n_;
    int connections_;
    int epoll_fd_;
    std::vector<int> sockets_;
    std::vector<std::pair<std::string, int>> expected_results_;

    void setupEpoll();

    void createConnections();

    void sendInFragments(int sock, const std::string& expr);

    void processResponses();
};

