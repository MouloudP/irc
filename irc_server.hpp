#ifndef IRC_SERVER_HPP
# define IRC_SERVER_HPP

# include "Include.hpp"
# include "ClientIRC.hpp"
# include "ChannelIRC.hpp"
# include "ChannelManager.hpp"
# include "CommandManager.hpp"

class ChannelManager;
class CommandManager;

class ServerIRC {
    public:
        ServerIRC();
        ServerIRC(int);
        ~ServerIRC();
        void Run();
        void Close();
        void MesssageReceived(ClientIRC *, std::string);
        void ExecuteCommand(ClientIRC *, std::string);
        ClientIRC *CreateClient();
    
    private:
        int port;
        int sockfd;
        fd_set current_sockets;
        fd_set ready_sockets;
        ChannelManager *channelManager;
        CommandManager *commandManager;

        /*vector of client*/
        std::vector <ClientIRC *> Clients;
};
#endif