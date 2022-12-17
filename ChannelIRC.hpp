#ifndef CHANNEL_IRC_HPP
# define CHANNEL_IRC_HPP

# include "Include.hpp"
# include "ChannelManager.hpp"
# include "ClientIRC.hpp"

class ClientIRC;
class ChannelManager;

class ChannelIRC {
    public:
        ChannelIRC(std::string, ChannelManager *);
        ~ChannelIRC();

        void SetName(std::string);
        std::string GetName();

        void SetTopic(std::string);
        std::string GetTopic();
        int getClientsCount(void);


        void AddClient(ClientIRC *);
        void RemoveClient(ClientIRC *);
        std::vector<ClientIRC *> GetClients();

        bool HasClient(ClientIRC *);
        void SendMessage(std::string, ClientIRC *);

        void SetMaxClients(int);
        int GetMaxClients();

        void SetBanPatern(std::string);
        std::string GetBanPatern();

        void SetModerated(bool);
        bool GetModerated();

        ClientIRC *getClientByName(std::string);
        void SetCantVoice(std::string, bool);
        bool GetCantVoice(std::string);

        /*void SetMode(std::string);
        std::string GetMode();*/

    private:
        std::string _name;
        std::string _topic;
        std::vector<ClientIRC *> _clients;
        ChannelManager *_channelManager;
        int _maxClients;
        std::string _banPatern;
        bool _moderated;
        std::map<std::string, bool> _cantVoice;
        //std::string mode;
};
#endif