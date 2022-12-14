#include "ChannelManager.hpp"

ChannelManager::ChannelManager() {}
ChannelManager::~ChannelManager() {}

ChannelIRC *ChannelManager::CreateChannel(std::string name, ClientIRC *client) {
    ChannelIRC *channel = new ChannelIRC(name, this);
    channels[name] = channel;
    
    std::cout << "CREATE CHANNEL |" << name << "|" << std::endl;
    channel->AddClient(client);
    //client->SendMessage(":mouloud 331 " + client->GetNick() + " " + name + " :No topic is set\r\n");
    return channel;
}

ChannelIRC *ChannelManager::GetChannel(std::string name) {
    std::cout << "I WANT THE CHANNELLLLLL |" << name << "|" << std::endl;

    for (std::map<std::string, ChannelIRC *>::iterator it = channels.begin(); it != channels.end(); it++)
        std::cout << "CHANNELS |" << it->first << "|" << std::endl;

    return channels[name];
}

void ChannelManager::DeleteChannel(std::string name) {
    delete channels[name];
    channels.erase(name);
}

void ChannelManager::DeleteChannel(ChannelIRC *channel) {
    for (std::map<std::string, ChannelIRC *>::iterator it = channels.begin(); it != channels.end(); it++) {
        if (it->second == channel) {
            channels.erase(it);
            break;
        }
    }
    delete channel;
}

void ChannelManager::DeleteAllChannels() {
    for (std::map<std::string, ChannelIRC *>::iterator it = channels.begin(); it != channels.end(); it++) {
        delete it->second;
    }
    channels.clear();
}

std::map<std::string, ChannelIRC *> ChannelManager::GetChannels() {
    return channels;
}

