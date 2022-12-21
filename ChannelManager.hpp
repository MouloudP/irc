#ifndef CHANNEL_MANAGER_HPP
# define CHANNEL_MANAGER_HPP
# include "ServerIRC.hpp"
# include "ChannelIRC.hpp"

class ChannelIRC;

class ChannelManager {
    public:
        ChannelManager();
        ~ChannelManager();
        ChannelIRC *CreateChannel(std::string, ClientIRC *);
        ChannelIRC *GetChannel(std::string);
        void DeleteChannel(std::string);
        void DeleteChannel(ChannelIRC *);
        void DeleteAllChannels();
        std::map<std::string, ChannelIRC *> GetChannels();
    
    private:
        std::map<std::string, ChannelIRC *> _channels;
};

#endif