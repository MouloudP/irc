#ifndef IRC_COMMAND_HPP
# define IRC_COMMAND_HPP

# include "irc_server.hpp"
# include "ClientIRC.hpp"
# include "ChannelManager.hpp"

class ChannelManager;
class ClientIRC;

class CommandManager {
    public:
        CommandManager(ChannelManager *);
        ~CommandManager();

        void ExecuteCommand(ClientIRC *, std::string);
        void Nick(ClientIRC *, std::vector<std::string>);
        void User(ClientIRC *, std::vector<std::string>);
        void Ping(ClientIRC *, std::vector<std::string>);
        void Join(ClientIRC *, std::vector<std::string>);
        void Part(ClientIRC *, std::vector<std::string>);
        void PrivMSG(ClientIRC *, std::vector<std::string>);
        void List(ClientIRC *, std::vector<std::string>);
        void Topic(ClientIRC *, std::vector<std::string>);
    private:
        ChannelManager *_channelManager;
};

#endif