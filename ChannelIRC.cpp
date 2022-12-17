#include "ChannelIRC.hpp"

ChannelIRC::ChannelIRC(std::string name, ChannelManager *_channelManager): _name(name), _channelManager(_channelManager), _maxClients(0), _moderated(false) {}

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

int			ChannelIRC::getClientsCount(void) {
    return (_clients.size());
}


void ChannelIRC::AddClient(ClientIRC *client) {
    _clients.push_back(client);

    std::cout << "_________________________" << std::endl;
    std::cout << client->GetNick() << std::endl;
    std::cout << "_________________________" << std::endl;

    std::string nameList = "";
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        std::string prefix = "";
        if ((*it)->GetOperator())
            prefix += "@";
        if (GetCantVoice((*it)->GetNick()))
            prefix += "+";
        nameList += prefix + (*it)->GetNick() + " ";
    }
    nameList = nameList.substr(0, nameList.size() - 1);

    std::cout << client->GetNick() << " JOIN " << _name << std::endl;
    this->SendMessage(":" + client->GetNick() + "!user@host " + " JOIN " + _name + "\r\n", client);\
    client->SendMessage(":mouloud 331 " + client->GetNick() + " " + _name + " :No topic is set\r\n");
    client->SendMessage(":mouloud 353 " + client->GetNick() + " = " + _name + " :" + nameList + "\r\n");
    client->SendMessage(":mouloud 366 " + client->GetNick() + " " + _name + " :End of /NAMES list.\r\n");
}

void ChannelIRC::RemoveClient(ClientIRC *client) {
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (*it == client) {
            std::cout << "REMOVE CLIENT FROM CHANNEL" << std::endl;
            //(*it)->SendMessage(":" + client->GetUserName() + "!127.0.0.1 " + " PART " + name + " leave chanel\r\n");
            _clients.erase(it);
            break;
        }
    }
    if (_clients.size() == 0) {
        std::cout << "DELETE CHANNEL" << std::endl;
        _channelManager->DeleteChannel(this);
    } else {
        //this->SendMessage(":" + client->GetUserName() + "!0 " + " PART " + _name + " leave chanel\r\n", NULL);
    }
}

bool ChannelIRC::HasClient(ClientIRC *client) {
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (*it == client) {
            return true;
        }
    }
    return false;
}

void ChannelIRC::SendMessage(std::string message, ClientIRC *client) {
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (*it == client) continue;
        (*it)->SendMessage(message);
    }
}

std::vector<ClientIRC *> ChannelIRC::GetClients() {
    return _clients;
}

void ChannelIRC::SetMaxClients(int maxClients) {
    _maxClients = maxClients;
}

int ChannelIRC::GetMaxClients() {
    return _maxClients;
}

void ChannelIRC::SetBanPatern(std::string banPatern) {
    _banPatern = banPatern;
}

std::string ChannelIRC::GetBanPatern() {
    return _banPatern;
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