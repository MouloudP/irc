#include "ChannelIRC.hpp"

ChannelIRC::ChannelIRC(std::string name, ChannelManager *channelManager): name(name), channelManager(channelManager) {}

ChannelIRC::~ChannelIRC() {}

void ChannelIRC::SetName(std::string name) {
    this->name = name;
}

std::string ChannelIRC::GetName() {
    return name;
}

void ChannelIRC::SetTopic(std::string topic) {
    this->topic = topic;
}

std::string ChannelIRC::GetTopic() {
    return topic;
}

void ChannelIRC::AddClient(ClientIRC *client) {
    clients.push_back(client);
}

void ChannelIRC::RemoveClient(ClientIRC *client) {
    for (std::vector<ClientIRC *>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (*it == client) {
            std::cout << "REMOVE CLIENT FROM CHANNEL" << std::endl;
            (*it)->SendMessage("PART " + name + "\r\n");
            clients.erase(it);
            break;
        }
    }
    if (clients.size() == 0) {
        std::cout << "DELETE CHANNEL" << std::endl;
        channelManager->DeleteChannel(this);
    }
}

std::vector<ClientIRC *> ChannelIRC::GetClients() {
    return clients;
}

ChannelManager::ChannelManager() {}
ChannelManager::~ChannelManager() {}

ChannelIRC *ChannelManager::CreateChannel(std::string name, ClientIRC *client) {
    ChannelIRC *channel = new ChannelIRC(name, this);
    channels[name] = channel;

    channel->AddClient(client);
    client->SendMessage(":mouloud 331 " + client->GetNick() + " " + name + " :No topic is set\r\n");
    return channel;
}

ChannelIRC *ChannelManager::GetChannel(std::string name) {
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

