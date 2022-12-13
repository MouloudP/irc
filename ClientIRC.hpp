#ifndef CLIENT_IRC_HPP
# define CLIENT_IRC_HPP
# include "Include.hpp"

class ClientIRC {
    public:
        ClientIRC(int);
        ~ClientIRC();
        int GetFd();
        void SendMessage(const std::string);
        void ClientConnect();

        void SetNick(std::string);
        std::string GetNick();

        void SetUserName(std::string);
        std::string GetUserName();

        void SetRealName(std::string);
        std::string GetRealName();

        void SetRegistered(bool);
        bool GetRegistered();
    
    private:
        int _fd;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        bool _registered;
};
#endif