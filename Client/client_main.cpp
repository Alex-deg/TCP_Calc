#include "client.h"

int main(int argc, char* argv[]) {
    
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <n> <connections> <server_addr> <server_port>" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        int n = std::stoi(argv[1]);
        int connections = std::stoi(argv[2]);
        std::string server_addr = argv[3];
        int server_port = std::stoi(argv[4]);

        TCPClient client(server_addr, server_port, n, connections);
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}