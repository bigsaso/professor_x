#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

bool receiveData(int sockfd, struct sockaddr_in &client_addr, double* buffer, int bufferSize) {
    socklen_t addr_len = sizeof(client_addr);
    int expectedSize = bufferSize * sizeof(double); // Size in bytes

    ssize_t recv_len = recvfrom(sockfd, (char*) buffer, expectedSize, 0, 
                                (struct sockaddr *) &client_addr, &addr_len);

    if (recv_len < 0) {
        std::cerr << "recvfrom failed" << std::endl;
        return false;
    } else if (recv_len != expectedSize) {
        std::cerr << "Received unexpected data size: " << recv_len << std::endl;
        return false;
    }

    return true;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error opening socket" << std::endl;
        return 1;
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT); // Port number

    // Bind
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    double buffer[1024];

    while (true) {
        std::cout << "Waiting for data..." << std::endl;

        if (receiveData(sockfd, client_addr, buffer, 1024)) {
            std::cout << "Received data: " << std::endl;
            for (int i = 0; i < 1024; i++) {
                std::cout << buffer[i] << ", ";
            }
            std::cout << std::endl;
        }
    }

    close(sockfd);

    return 0;
}