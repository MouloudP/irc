#include "ClientIRC.hpp"

ClientIRC::ClientIRC(int fd): fd(fd), registered(false) {
    std::cout << "ClientIRC::ClientIRC(int)" << std::endl;
}

ClientIRC::~ClientIRC() {
    close(fd);

    std::cout << "ClientIRC::~ClientIRC()" << std::endl;
}

int ClientIRC::GetFd() {
    return fd;
}

void ClientIRC::SendMessage(const std::string msg) {
    const char *message = msg.c_str();
    send(fd, message, strlen(message), 0);
}

void ClientIRC::SetNick(std::string nick) {
    nickname = nick;
}

std::string ClientIRC::GetNick() {
    return nickname;
}

void ClientIRC::SetUserName(std::string username) {
    this->username = username;
}

std::string ClientIRC::GetUserName() {
    return username;
}

void ClientIRC::SetRealName(std::string realname) {
    this->realname = realname;
}

std::string ClientIRC::GetRealName() {
    return realname;
}

void ClientIRC::SetRegistered(bool registered) {
    this->registered = registered;
}

bool ClientIRC::GetRegistered() {
    return registered;
}