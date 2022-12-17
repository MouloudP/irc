#include "ClientIRC.hpp"

ClientIRC::ClientIRC(int fd): _fd(fd), _registered(false) {
    std::cout << "ClientIRC::ClientIRC(int)" << std::endl;
}

ClientIRC::~ClientIRC() {
    close(_fd);

    std::cout << "ClientIRC::~ClientIRC()" << std::endl;
}

int ClientIRC::GetFd() {
    return _fd;
}

void ClientIRC::SendMessage(const std::string msg) {
    const char *message = msg.c_str();
    send(_fd, message, strlen(message), 0);
}

void ClientIRC::SetNick(std::string nick) {
    _nickname = nick;
}

std::string ClientIRC::GetNick() {
    return _nickname;
}

void ClientIRC::SetUserName(std::string username) {
    this->_username = username;
}

std::string ClientIRC::GetUserName() {
    return _username;
}

void ClientIRC::SetRealName(std::string realname) {
    this->_realname = realname;
}

std::string ClientIRC::GetRealName() {
    return _realname;
}

void ClientIRC::SetRegistered(bool registered) {
    this->_registered = registered;
}

bool ClientIRC::GetRegistered() {
    return _registered;
}

void ClientIRC::SetOperator(bool op) {
    this->_operator = op;
}

bool ClientIRC::GetOperator() {
    return _operator;
}