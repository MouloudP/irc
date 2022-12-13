#ifndef IRC_SERVER_HPP
# define IRC_SERVER_HPP


# include "Include.hpp"
# include "ClientIRC.hpp"
# include "ChannelIRC.hpp"
# include "ChannelManager.hpp"

class ChannelManager;

class ServerIRC {
    public:
        ServerIRC();
        ServerIRC(int, std::string);
        ~ServerIRC();


        int				getPort(void) const;
		std::string		getPassword(void) const;
        void Run();
        void Close();
        void MesssageReceived(ClientIRC *, std::string);
        void ExecuteCommand(ClientIRC *, std::string);
        ClientIRC *CreateClient();
    
    private:
        int _port;
        int _sockfd;
        fd_set _currentSockets;
        std::string _password;
        fd_set _readySockets;
        ChannelManager *_channelManager;
        
        /*vector of client*/
        std::vector <ClientIRC *> _clients;
};
#endif