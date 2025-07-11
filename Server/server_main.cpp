#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <poll.h>
#include <unordered_map>

const unsigned int PORT = 8888;
const unsigned int BUFFER_SIZE = 1024;

int main(){

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&address, sizeof(address));

    //set_nonblocking(server_fd);

    listen(server_fd, 1 /*MAX_CLIENTS*/);

    std::vector<pollfd> fds;

    pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    fds.push_back(server_pollfd);

    std::unordered_map<int, std::vector<char>> client_buffers;

}