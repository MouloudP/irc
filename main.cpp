#include "irc_server.hpp"

//ServerIRC server;

/*void signalKill(int sig) {
    server.Close();
    std::cout << "TURNING OFF" << std::endl;
    exit(0);
}*/

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    //signal(SIGINT, signalKill);
    ServerIRC server(atoi(argv[1]));
    server.Run();
    

    /*int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(6667);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    while (1) {
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int clientfd = accept(sockfd, (struct sockaddr *)&client, &client_len);

        if (clientfd == -1) {
            perror("accept");
            exit(1);
        }

        // Read the request from the client
        char request[1024];
        int bytes_received = recv(clientfd, request, sizeof(request), 0);

        // print the request to the console
        std::cout << request << std::endl;

        if (bytes_received == -1) {
            perror("recv");
            exit(1);
        }

        send(clientfd, "HTTP/1.1 200 OK\r", 16, 0);

        close(clientfd);
    }*/

    return 0;
}