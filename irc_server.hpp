#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <signal.h>
#include <fcntl.h>

class ClientIRC {
    public:
        ClientIRC(int);
        ~ClientIRC();
        int GetFd();
        void SendMessage(const char *);
        void ClientConnect();
    
    private:
        int fd;
};

class ServerIRC {
    public:
        ServerIRC();
        ServerIRC(int);
        ~ServerIRC();
        void Run();
        void Close();
        void MesssageReceived(ClientIRC *, std::string);
        ClientIRC *CreateClient();
    
    private:
        int port;
        int sockfd;
        fd_set current_sockets;
        fd_set ready_sockets;

        /*vector of client*/
        std::vector <ClientIRC *> Clients;
};