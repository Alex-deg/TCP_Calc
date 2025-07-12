#pragma once

class TCPServer {
public:
    TCPServer(int port) : port_(port), epoll_fd_(-1), server_fd_(-1);
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
    int calculateExpression(const std::string& expr);
};