#include "irc_server.hpp"

ServerIRC::ServerIRC() {
    std::cout << "ServerIRC::ServerIRC()" << std::endl;
}

ServerIRC::ServerIRC(int port, std::string s): _port(port), _password(s) {
    std::cout << "ServerIRC::ServerIRC(int)" << std::endl;

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1) {
        perror("socket");
        exit(1);
    }

    int yes = 1;

    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(_sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(_sockfd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    _channelManager = new ChannelManager();
    _commandManager = new CommandManager(_channelManager, this);
}

ServerIRC::~ServerIRC() {
    close(_sockfd);

    std::cout << "ServerIRC::~ServerIRC()" << std::endl;
}

int     ServerIRC::getPort(void) const {return (_port);}
std::string ServerIRC::getPassword(void) const {return (_password);}
std::vector<ClientIRC *> ServerIRC::getClients() {return (_clients);}

ClientIRC *ServerIRC::GetClientByNick(std::string nick) {
    for (auto it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->GetNick() == nick)
            return (*it);
    }
    return (NULL);
}

ClientIRC *ServerIRC::CreateClient() {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    std::cout << _sockfd << std::endl;
    int clientfd = accept(_sockfd, (struct sockaddr *)&client, &client_len);

    if (clientfd == -1) {
        perror("accept");
        exit(1);
    }

    FD_SET(clientfd, &_currentSockets);

    ClientIRC *client_irc = new ClientIRC(clientfd);
    _clients.push_back(client_irc);

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

void ServerIRC::RemoveClient(ClientIRC *client) {
    std::vector<ClientIRC *> newClients;
    for (auto it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it)->GetFd() == client->GetFd()) {
            int fd = client->GetFd();
            close(fd);
            FD_CLR(fd, &_currentSockets);
        } else {
            newClients.push_back(*it);
        }
    }
    std::cout << "Client removed" << std::endl;
    _clients = newClients;
    std::cout << "Client removed2" << std::endl;
}

void ServerIRC::Run() {
    FD_ZERO(&_currentSockets);
    FD_SET(_sockfd, &_currentSockets);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    while (true) {
        _readySockets = _currentSockets;
        if (select(FD_SETSIZE, &_readySockets, NULL, NULL, &timeout) < 0) {
            perror("select");
            exit(1);
        }

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &_readySockets)) {
                if (i == _sockfd) {
                    ClientIRC *test = this->CreateClient();
                } else {
                    FD_CLR(i, &_currentSockets);
                }
            }
        }

        // Iteration on Clients
        for (auto it = _clients.begin(); it != _clients.end(); ++it) {
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
            if (!request.empty()) {
                MesssageReceived((*it), request);
            }
            buffer[0] = '\0';
        }
    }
}

void ServerIRC::Close() {
    close(_sockfd);
    for (auto it = _clients.begin(); it != _clients.end(); ++it) {
        close((*it)->GetFd());
        delete *it;
    }
}

void ServerIRC::MesssageReceived(ClientIRC *client, std::string message) {
    //std::cout << "RECEIVE FROM [" << client->GetFd() << "] : " << message << std::endl;

    std::vector<std::string> messages = splitString(message, "\n");
    for (auto it = messages.begin(); it != messages.end(); ++it) {
        this->_commandManager->ExecuteCommand(client, *it);
    }
}