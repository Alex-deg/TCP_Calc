#include "server.h"

TCPServer::TCPServer(int port){
    this->port_ = port;
    epoll_fd_ = -1;
    server_fd_ = -1;
    createSocket();
    setupEpoll();
}

TCPServer::~TCPServer() {
    if (server_fd_ != -1) close(server_fd_);
    if (epoll_fd_ != -1) close(epoll_fd_);
}

void TCPServer::run() {
    std::cout << "Server listening on port " << port_ << "..." << std::endl;
    
    while (true) {
        int nfds = epoll_wait(epoll_fd_, events_.data(), MAX_EVENTS, -1);
        if (nfds == -1) {
            throw std::runtime_error("epoll_wait failed");
        }
        for (int i = 0; i < nfds; ++i) {
            if (events_[i].data.fd == server_fd_) {
                client_request = "";
                handleNewConnection();
            } else {
                handleClientData(events_[i].data.fd);
                // if(!client_request.empty()){
                //     try {
                //         int result = calculateExpression(client_request);
                //         std::string response = std::to_string(result);
                //         send(events_[i].data.fd, response.c_str(), response.size(), 0);
                //         std::cout << "Sent result: " << response << std::endl;
                //     } catch (const std::exception& e) {
                //         std::cerr << "Error calculating expression: " << e.what() << std::endl;
                //         std::string error = "Error: " + std::string(e.what());
                //         send(events_[i].data.fd, error.c_str(), error.size(), 0);
                //     }
                // }
            }
        }
    }
}

void TCPServer::createSocket() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        throw std::runtime_error("socket creation failed");
    }

    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        throw std::runtime_error("setsockopt failed");
    }

    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address))) {
        throw std::runtime_error("bind failed");
    }

    set_nonblocking(server_fd_);

    if (listen(server_fd_, 10) < 0) {
        throw std::runtime_error("listen failed");
    }
}

void TCPServer::setupEpoll() {
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) {
        throw std::runtime_error("epoll_create1 failed");
    }

    struct epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = server_fd_;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, server_fd_, &ev) == -1) {
        throw std::runtime_error("epoll_ctl: server_fd failed");
    }
}

void TCPServer::handleNewConnection() {
    struct sockaddr_in client_addr{};
    socklen_t addrlen = sizeof(client_addr);
    int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &addrlen);
    if (client_fd < 0) {
        std::cerr << "accept failed" << std::endl;
        return;
    }

    set_nonblocking(client_fd);

    std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) 
                << ":" << ntohs(client_addr.sin_port) << std::endl;

    struct epoll_event ev{};
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        std::cerr << "epoll_ctl: client_fd failed" << std::endl;
        close(client_fd);
    }
}

void TCPServer::handleClientData(int fd) {
    char buffer[BUF_SIZE] = {0};
    ssize_t valread;
    // Читаем данные, пока не получим весь запрос
    while (true) {
        valread = recv(fd, buffer, BUF_SIZE, 0);
        if (valread == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Данные пока не готовы, выходим из цикла
                return;
            } else {
                // Ошибка чтения
                std::cerr << "recv failed" << std::endl;
                epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
                close(fd);
                break;
            }
        } else if (valread == 0) {
            // Клиент закрыл соединение
            epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
            close(fd);
            break;
        }

        // Добавляем полученные данные к выражению
        buffer[valread] = '\0';  // Гарантируем нуль-терминацию
        client_request += buffer;
        if(client_request.find(' ') != std::string::npos){
            std::cout << "Передача данных закончена" << std::endl;
            break;
        }
    }

    // Проверяем, есть ли что вычислять
    if (client_request.empty()) {
        return;
    }

    std::cout << "Received expression: " << client_request << std::endl;

    try {
        int result = Calculator::calculateExpression(client_request);
        std::string response = std::to_string(result);
        send(fd, response.c_str(), response.size(), 0);
        std::cout << "Sent result: " << response << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error calculating expression: " << e.what() << std::endl;
        std::string error = "Error: " + std::string(e.what());
        send(fd, error.c_str(), error.size(), 0);
    }
}

int TCPServer::set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
