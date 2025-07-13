#include "server.h"

// const unsigned int PORT = 8888;
// const unsigned int BUFFER_SIZE = 1024;

//   /home/alex/Протей/ДЗ №6 Сеть/TCP_Calculator/build

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        int port = std::stoi(argv[1]);
        TCPServer server(port);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// int main(){
//
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//
//     sockaddr_in address;
//     memset(&address, 0, sizeof(address));
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);
//
//     bind(server_fd, (sockaddr*)&address, sizeof(address));
//
//     //set_nonblocking(server_fd);
//
//     listen(server_fd, 1 /*MAX_CLIENTS*/);
//
//     std::vector<pollfd> fds;
//
//     pollfd server_pollfd;
//     server_pollfd.fd = server_fd;
//     server_pollfd.events = POLLIN;
//     server_pollfd.revents = 0;
//     fds.push_back(server_pollfd);
//
//     std::unordered_map<int, std::vector<char>> client_buffers;
//
//     while(true) {
//         poll(fds.data(), fds.size(), 0/*POLL_TIMEOUT*/);
//
//         for(size_t i = 0; i < fds.size();){
//             pollfd& pfd = fds[i];
//             if(pfd.fd == server_fd && (pfd.revents & POLLIN)){
//                 while(true){
//                     sockaddr_in client_addr;
//                     socklen_t addrlen = sizeof(client_addr);
//                     int client_fd = accept(server_fd, (sockaddr*)&client_addr, &addrlen);
//                     if(client_fd < 0){
//                         if(errno == EAGAIN || errno == EWOULDBLOCK){
//                             break;
//                         }
//                     }
//                     //set_nonblocking(client_fd);
//                     pollfd client_pollfd;
//                     client_pollfd.fd = client_fd;
//                     client_pollfd.events = POLLIN;
//                     client_pollfd.revents = 0;
//                     fds.push_back(client_pollfd);
//
//                     client_buffers[client_fd] = std::vector<char>();
//                 }
//             }
//             else if(pfd.revents & POLLIN){
//                 int client_fd = pfd.fd;
//                 char buffer[BUFFER_SIZE];
//                 while(true){
//                     ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
//                     if(bytes_read > 0){
//                         client_buffers[client_fd].insert(
//                             client_buffers[client_fd].end(),
//                             buffer,
//                             buffer + bytes_read
//                         );
//                         pfd.events |= POLLOUT;
//                     } else{
//                         break;
//                     }
//                 }
//                 else if (pfd.revents & POLLOUT){
//                     int  client_fd = pfd.fd;
//                     auto& buffer = client_buffers[client_fd];
//                     if(!buffer.empty()){
//                         ssize_t bytes_sent = send(client_fd, buffer.data(), buffer.size(), MSG_MOSIGNAL);
//
//                         if(bytes_sent > 0){
//                             buffer.erase(buffer.begin(), buffer().begin() + bytes_sent);
//                             if(buffer.empty()){
//                                 pfd.events &= ~POLLOUT;
//                             }
//                         }
//                     }
//                     else{
//                         pfd.events &= ~POLLOUT;
//                     }
//                 }
//                 pfd.revents = 0;
//                 i++;
//             }
//         }
//     }
//
// }