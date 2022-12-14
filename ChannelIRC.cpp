#include "ChannelIRC.hpp"

ChannelIRC::ChannelIRC(std::string name, ChannelManager *channelManager, ClientIRC *owner): _name(name), _channelManager(channelManager), _maxClients(0), _moderated(false), _owner(owner) {}

ChannelIRC::~ChannelIRC() {}

void ChannelIRC::SetName(std::string name) {
    _name = name;
}

std::string ChannelIRC::GetName() {
    return _name;
}

void ChannelIRC::SetTopic(std::string topic) {
    _topic = topic;
}

std::string ChannelIRC::GetTopic() {
    return _topic;
}

int ChannelIRC::getClientsCount(void) {
    return (_clients.size());
}


void ChannelIRC::AddClient(ClientIRC *client) {
    _clients.push_back(client);

    std::string nameList = "";
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        std::string prefix = "";
        if ((*it) == _owner)
            prefix += "@";
        else if (GetCantVoice((*it)->GetNick()))
            prefix += "+";
        nameList += prefix + (*it)->GetNick() + " ";
    }
    nameList = nameList.substr(0, nameList.size() - 1);

    this->SendMessage(":" + client->GetNick() + "!user@host JOIN " + _name + "\r\n", NULL);
    client->SendMessage(":ircserver 353 " + client->GetNick() + " = " + _name + " :" + nameList + "\r\n");
    client->SendMessage(":ircserver 366 " + client->GetNick() + " " + _name + " :End of /NAMES list.\r\n");

    if (_topic != "")
        client->SendMessage(":ircserver 332 " + client->GetNick() + " " + _name + " :" + _topic + "\r\n");
    else
        client->SendMessage(":ircserver 331 " + client->GetNick() + " " + _name + " :No topic is set\r\n");
}

void ChannelIRC::RemoveClient(ClientIRC *client) {
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (*it == client) {
            _clients.erase(it);
            break;
        }
    }
    if (_clients.size() == 0) {
        std::cout << "DELETE CHANNEL" << std::endl;
        _channelManager->DeleteChannel(this);
    }
}

bool ChannelIRC::HasClient(ClientIRC *client) {
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (!client->GetKilled() && *it == client) {
            return true;
        }
    }
    return false;
}

void ChannelIRC::SendMessage(std::string message, ClientIRC *client) {
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if ((*it)->GetKilled() || *it == client) continue;
        (*it)->SendMessage(message);
    }
}

std::vector<ClientIRC *> ChannelIRC::GetClients() {
    return _clients;
}

void ChannelIRC::SetMaxClients(int maxClients) {
    _maxClients = maxClients;
}

unsigned long ChannelIRC::GetMaxClients() {
    return _maxClients;
}

void ChannelIRC::SetModerated(bool moderated) {
    _moderated = moderated;
}

bool ChannelIRC::GetModerated() {
    return _moderated;
}

ClientIRC *ChannelIRC::getClientByName(std::string name) {
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if ((*it)->GetNick() == name) {
            return (*it);
        }
    }
    return NULL;
}

void ChannelIRC::SetCantVoice(std::string cantVoice, bool value) {
    _cantVoice[cantVoice] = value;
}

bool ChannelIRC::GetCantVoice(std::string cantVoice) {
    return _cantVoice[cantVoice];
}

void ChannelIRC::SetOwner(ClientIRC *owner) {
    _owner = owner;
}

ClientIRC *ChannelIRC::GetOwner() {
    return _owner;
}
