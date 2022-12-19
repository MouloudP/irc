#ifndef IRC_COMMAND_HPP
# define IRC_COMMAND_HPP

# include "irc_server.hpp"
# include "ClientIRC.hpp"
# include "ChannelManager.hpp"

class ChannelManager;
class ClientIRC;
class ServerIRC;

class CommandManager {
    public:
        CommandManager(ChannelManager *, ServerIRC *);
        ~CommandManager();

        void ExecuteCommand(ClientIRC *, std::string);
        void Pass(ClientIRC *, std::vector<std::string>);
        void Nick(ClientIRC *, std::vector<std::string>);
        void User(ClientIRC *, std::vector<std::string>);
        void Ping(ClientIRC *, std::vector<std::string>);
        void Join(ClientIRC *, std::vector<std::string>);
        void Part(ClientIRC *, std::vector<std::string>);
        void PrivMSG(ClientIRC *, std::vector<std::string>);
        void List(ClientIRC *, std::vector<std::string>);
        void Topic(ClientIRC *, std::vector<std::string>);
        void Mode(ClientIRC *, std::vector<std::string>);
        void Restart(ClientIRC *, std::vector<std::string>);
        void Kill(ClientIRC *, std::vector<std::string>);
        void Oper(ClientIRC *, std::vector<std::string>);
        void Kick(ClientIRC *, std::vector<std::string>);
        void Quit(ClientIRC *, std::vector<std::string>);

    private:
        ChannelManager *_channelManager;
        ServerIRC *_server;
};

#endif
