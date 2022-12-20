#include "ChannelManager.hpp"

ChannelManager::ChannelManager() {}
ChannelManager::~ChannelManager() {}

ChannelIRC *ChannelManager::CreateChannel(std::string name, ClientIRC *client) {
    ChannelIRC *channel = new ChannelIRC(name, this, client);
    _channels[name] = channel;
    
    std::cout << "CREATE CHANNEL |" << name << "|" << std::endl;
    channel->AddClient(client);
    //client->SendMessage(":mouloud 331 " + client->GetNick() + " " + name + " :No topic is set\r\n");
    return channel;
}

ChannelIRC *ChannelManager::GetChannel(std::string name) {
    //std::cout << "I WANT THE CHANNELLLLLL |" << name << "|" << std::endl;

    /*for (std::map<std::string, ChannelIRC *>::iterator it = _channels.begin(); it != _channels.end(); it++)
        std::cout << "CHANNEL |" << it->first << "|" << std::endl;*/

    return _channels[name];
}

void ChannelManager::DeleteChannel(std::string name) {
    delete _channels[name];
    _channels.erase(name);
}

void ChannelManager::DeleteChannel(ChannelIRC *channel) {
    for (std::map<std::string, ChannelIRC *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
        if (it->second == channel) {
            _channels.erase(it);
            break;
        }
    }
    delete channel;
}

void ChannelManager::DeleteAllChannels() {
    for (std::map<std::string, ChannelIRC *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
        delete it->second;
    }
    _channels.clear();
}

std::map<std::string, ChannelIRC *> ChannelManager::GetChannels() {
    return _channels;
}

