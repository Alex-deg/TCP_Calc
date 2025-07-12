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

class TCPClient {
public:
    TCPClient(const std::string& server_addr, int server_port, int n, int connections)
        : server_addr_(server_addr), server_port_(server_port), n_(n), connections_(connections),
          epoll_fd_(-1) {
        
        if (n < 2) {
            throw std::invalid_argument("n must be at least 2");
        }
        if (connections < 1) {
            throw std::invalid_argument("connections must be at least 1");
        }
    }

    ~TCPClient() {
        if (epoll_fd_ != -1) close(epoll_fd_);
    }

    void run() {
        setupEpoll();
        createConnections();
        processResponses();
    }

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

    void setupEpoll() {
        epoll_fd_ = epoll_create1(0);
        if (epoll_fd_ == -1) {
            throw std::runtime_error("epoll_create1 failed");
        }
    }

    void createConnections() {
        struct sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(server_port_);
        
        if (inet_pton(AF_INET, server_addr_.c_str(), &serv_addr.sin_addr) <= 0) {
            throw std::runtime_error("invalid address");
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> num_dist(1, 100);
        std::uniform_int_distribution<> op_dist(0, 3);
        const char ops[] = {'+', '-', '*', '/'};

        for (int i = 0; i < connections_; ++i) {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                std::cerr << "socket creation failed for connection " << i+1 << std::endl;
                continue;
            }

            if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                std::cerr << "connection failed for connection " << i+1 << std::endl;
                close(sock);
                continue;
            }

            // Generate expression
            std::ostringstream expr;
            expr << num_dist(gen);
            int expected = std::stoi(expr.str());
            
            for (int j = 1; j < n_; ++j) {
                char op = ops[op_dist(gen)];
                int num = num_dist(gen);
                expr << " " << op << " " << num;
                
                switch (op) {
                    case '+': expected += num; break;
                    case '-': expected -= num; break;
                    case '*': expected *= num; break;
                    case '/': 
                        if (num != 0) expected /= num; 
                        else expected = 0;
                        break;
                }
            }

            std::string expression = expr.str();
            expected_results_.emplace_back(expression, expected);
            
            // Send expression in fragments
            sendInFragments(sock, expression);
            
            // Add socket to epoll
            struct epoll_event ev{};
            ev.events = EPOLLIN;
            ev.data.fd = sock;
            if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, sock, &ev) == -1) {
                std::cerr << "epoll_ctl failed for connection " << i+1 << std::endl;
                close(sock);
                continue;
            }

            sockets_.push_back(sock);
            std::cout << "Connection " << i+1 << ": Sent expression: " << expression 
                      << " (expected: " << expected << ")" << std::endl;
        }
    }

    void sendInFragments(int sock, const std::string& expr) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> frag_dist(2, 5);
        
        int fragments = frag_dist(gen);
        int fragment_size = expr.size() / fragments;
        
        for (int i = 0; i < fragments; ++i) {
            int start = i * fragment_size;
            int size = (i == fragments - 1) ? (expr.size() - start) : fragment_size;
            
            send(sock, expr.c_str() + start, size, 0);
            usleep(10000); // Small delay between fragments
        }
    }

    void processResponses() {
        std::vector<struct epoll_event> events(MAX_EVENTS);
        
        while (!sockets_.empty()) {
            int nfds = epoll_wait(epoll_fd_, events.data(), MAX_EVENTS, -1);
            if (nfds == -1) {
                throw std::runtime_error("epoll_wait failed");
            }

            for (int i = 0; i < nfds; ++i) {
                int sock = events[i].data.fd;
                char buffer[BUF_SIZE] = {0};
                int valread = read(sock, buffer, BUF_SIZE);
                
                if (valread <= 0) {
                    // Connection closed or error
                    auto it = std::find(sockets_.begin(), sockets_.end(), sock);
                    if (it != sockets_.end()) {
                        size_t index = it - sockets_.begin();
                        expected_results_.erase(expected_results_.begin() + index);
                        sockets_.erase(it);
                    }
                    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, sock, nullptr);
                    close(sock);
                    continue;
                }

                // Find which connection this is
                auto it = std::find(sockets_.begin(), sockets_.end(), sock);
                if (it == sockets_.end()) continue;
                
                size_t index = it - sockets_.begin();
                int server_result = std::stoi(buffer);
                int expected = expected_results_[index].second;
                
                if (server_result != expected) {
                    std::cerr << "ERROR: Expression: " << expected_results_[index].first 
                              << " | Server result: " << server_result 
                              << " | Expected: " << expected << std::endl;
                } else {
                    std::cout << "OK: Expression: " << expected_results_[index].first 
                              << " | Result: " << server_result << std::endl;
                }
                
                // Remove this connection
                expected_results_.erase(expected_results_.begin() + index);
                sockets_.erase(it);
                epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, sock, nullptr);
                close(sock);
            }
        }
    }
};

