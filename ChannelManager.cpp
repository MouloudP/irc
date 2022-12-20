#include "ChannelManager.hpp"

ChannelManager::ChannelManager() {}
ChannelManager::~ChannelManager() {
    DeleteAllChannels();
}

ChannelIRC *ChannelManager::CreateChannel(std::string name, ClientIRC *client) {
    ChannelIRC *channel = new ChannelIRC(name, this, client);
    _channels[name] = channel;
    channel->AddClient(client);
    return channel;
}

ChannelIRC *ChannelManager::GetChannel(std::string name) {
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
        if (!it->second) continue;
        delete it->second;
    }
    _channels.clear();
}

std::map<std::string, ChannelIRC *> ChannelManager::GetChannels() {
    return _channels;
}

