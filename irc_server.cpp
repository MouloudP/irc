#include "irc_server.hpp"

ServerIRC::ServerIRC() {
    std::cout << "ServerIRC::ServerIRC()" << std::endl;
}

ServerIRC::ServerIRC(int port): port(port) {
    std::cout << "ServerIRC::ServerIRC(int)" << std::endl;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    int yes = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    channelManager = new ChannelManager();
    commandManager = new CommandManager(channelManager);
}

ServerIRC::~ServerIRC() {
    close(sockfd);

    std::cout << "ServerIRC::~ServerIRC()" << std::endl;
}

ClientIRC *ServerIRC::CreateClient() {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    std::cout << sockfd << std::endl;
    int clientfd = accept(sockfd, (struct sockaddr *)&client, &client_len);

    if (clientfd == -1) {
        perror("accept");
        exit(1);
    }

    FD_SET(clientfd, &current_sockets);

    ClientIRC *client_irc = new ClientIRC(clientfd);
    Clients.push_back(client_irc);

    /*for (auto it = Clients.begin(); it != Clients.end(); ++it) {
        std::cout << (*it)->GetFd() << std::endl;
    }*/

    //std::cout << Clients.size() << std::endl;

    /*Protocol connection*/
    std::cout << "[" << client_irc->GetFd() << "]" << "Client connected" << std::endl;

    if (fcntl(client_irc->GetFd(), F_SETFL, fcntl(client_irc->GetFd(), F_GETFL) | O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(1);
    }
    return client_irc;
}

void ServerIRC::Run() {
    FD_ZERO(&current_sockets);
    FD_SET(sockfd, &current_sockets);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    while (true) {
        ready_sockets = current_sockets;
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, &timeout) < 0) {
            perror("select");
            exit(1);
        }

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &ready_sockets)) {
                if (i == sockfd) {
                    ClientIRC *test = this->CreateClient();
                } else {
                    FD_CLR(i, &current_sockets);
                }
            }
        }

        // Iteration on Clients
        for (auto it = Clients.begin(); it != Clients.end(); ++it) {
           //Non blocking socket

            //std::cout << "START READ MESSAGE" << (*it)->GetFd() << std::endl;
            //char request[1024];
            //int bytes_received = recv((*it)->GetFd(), request, sizeof(request), 0);

            std::string request = "";
            std::string dataRead = "";
            char buffer[1024];

            int lenght = recv((*it)->GetFd(), buffer, sizeof(buffer), 0);
            if (lenght > 0) {
                buffer[lenght] = '\0';
                //std::cout << "\\BUFFER = " << buffer << std::endl;
                fflush(stdout);
                request = buffer;
            }

            /*while (request.empty() && (lenght) > 0) {
                dataRead += buffer;
                request = dataRead;
            }*/
            if (!request.empty()) {
                //std::cout << "---------------------------" << std::endl;
                //std::cout << "[" << (*it)->GetFd() << "]" << request << std::endl;
                //std::cout << "---------------------------" << std::endl;
                /*if (request.find("USER") != std::string::npos) {
                    std::cout << "USER CONNECTED !!!!" << std::endl;
                    (*it)->SendMessage(":mouloud 001 ahamdoun :Welcome to the Internet Relay Network\n");
                }*/
                MesssageReceived((*it), request);
            }
            buffer[0] = '\0';
            // print the request to the console
            //std::cout << "[" << (*it)->GetFd() << "]" << request << std::endl;

            /*Split by space*/
            //char *token = strtok(request, " ");
            //std::string command = token;
            /*std::string *arg = new std::string[10];
            for (int i = 1; token[i] != NULL; i++) {
                arg[i] = token[i];
            }*/
            /*if (command == "JOIN") {
                std::cout << "JOIN" << std::endl;
            }
            if (command == "NICK" || command == "CAP") {
                (*it)->SendMessage(":mouloud 001 ahamdoun :Welcome to the Internet Relay Network\n");
                std::cout << "send 001" << std::endl;
            }*/

            //std::cout << "END READ MESSAGE" << (*it)->GetFd() << std::endl;
        }
    }
}

void ServerIRC::Close() {
    close(sockfd);
    for (auto it = Clients.begin(); it != Clients.end(); ++it) {
        close((*it)->GetFd());
        delete *it;
    }
}

void ServerIRC::MesssageReceived(ClientIRC *client, std::string message) {
    //std::cout << "RECEIVE FROM [" << client->GetFd() << "] : " << message << std::endl;

    std::vector<std::string> messages = splitString(message, "\n");
    for (auto it = messages.begin(); it != messages.end(); ++it) {
        this->commandManager->ExecuteCommand(client, *it);
    }
}

/*void ServerIRC::ExecuteCommand(ClientIRC *client, std::string command) {
    std::cout << "MESSAGE RECEIVED : " << command << std::endl;

    std::vector<std::string> args = splitString(command, " ");
    std::cout << "COMMAND : " << args[0] << std::endl;

    if (args[args.size() - 1].find("\r") != std::string::npos) {
        args[args.size() - 1].erase(args[args.size() - 1].find("\r"), 1);
    }
    if (args[args.size() - 1].find("\n") != std::string::npos) {
        args[args.size() - 1].erase(args[args.size() - 1].find("\n"), 1);
    }
    this->commandManager->ExecuteCommand(client, args);
}*/