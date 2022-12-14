#include "CommandManager.hpp"

CommandManager::CommandManager(ChannelManager* _channelManager, ServerIRC* _server): _channelManager(_channelManager), _server(_server) {}
CommandManager::~CommandManager() {}

std::vector<std::string> splitString(std::string str, std::string delimiter) {
    std::vector<std::string> result;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    if (!str.empty()) {
        result.push_back(str);
    }
    return result;
}

std::string concatString(std::vector<std::string> str, int start) {
    std::string result = "";
    for (unsigned long i = start; i < str.size(); i++) {
        result += str[i];
        if (i != str.size() - 1) {
            result += " ";
        }
    }
    return result;
}

void CommandManager::ExecuteCommand(ClientIRC *client, std::string command) {
    if (command.empty()) {
        return;
    }
    std::cout << "MESSAGE RECEIVED : " << command << std::endl;

    std::vector<std::string> args = splitString(command, " ");
    std::cout << "COMMAND : " << args[0] << std::endl;

    /*remove \r\n*/
    if (args[args.size() - 1].find("\r") != std::string::npos) {
        args[args.size() - 1].erase(args[args.size() - 1].find("\r"), 1);
    }
    if (args[args.size() - 1].find("\n") != std::string::npos) {
        args[args.size() - 1].erase(args[args.size() - 1].find("\n"), 1);
    }

    std::string commandName = args[0];
     if (commandName == "PASS") { 
        this->Pass(client, args);
    } else  if (commandName == "NICK") {
        this->Nick(client, args);
    } else if (commandName == "USER") {
        this->User(client, args);
    } else if (commandName == "CAP") {
        
    } else if (!client->GetRegistered()) {
        client->SendMessage(":ircserver 451 :You have not registered\n");
        return;
    } else if (commandName == "PING") {
        this->Ping(client, args);
    } else if (commandName == "JOIN") {
        this->Join(client, args);
    } else if (commandName == "PART") {
        this->Part(client, args);
    } else if (commandName == "PRIVMSG") {
        this->PrivMSG(client, args);
    } else if (commandName == "LIST") {
        this->List(client, args);
    } else if (commandName == "TOPIC") {
        this->Topic(client, args);
    } else if (commandName == "MODE") {
        this->Mode(client, args);
    } else if (commandName == "restart" || commandName == "RESTART") {
        this->Restart(client, args);
    } else if (commandName == "kill" || commandName == "KILL") {
        this->Kill(client, args);
    } else if (commandName == "OPER") {
        this->Oper(client, args);
    } else if (commandName == "KICK") {
        this->Kick(client, args);
    } else if (commandName == "QUIT") {
        this->Quit(client, args);
    } else {
        client->SendMessage(":ircserver 421 " + commandName +  ":Unknown command\n");
    }
}

void CommandManager::Pass(ClientIRC *client, std::vector<std::string> args) {
    std::string password = concatString(args, 1);
    if (_server->getClientSize() > 10) {
        client->SendMessage(":ircserver 451 :Too many clients connected\r\n");
        _server->RemoveClient(client);
    } else if (client->getPassword()) {
        client->SendMessage(":ircserver 462 :You may not reregister\n");
    } else if (args.size() < 2) {
        client->SendMessage(":ircserver 461 PASS :Not enough parameters\n");
         _server->RemoveClient(client);
    } else if (password == this->_server->getPassword()) {
        client->setPassword(true);
    } else {
        client->SendMessage(":ircserver 464 :Password incorrect\r\n");
        _server->RemoveClient(client);
    }
}   

void CommandManager::Nick(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 2) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " NICK :Not enough parameters\r\n");
        return;
    }

    ClientIRC *client2 = this->_server->GetClientByNick(args[1]);
    if (client2 != NULL) {
        client->SendMessage("Nickname already in use\r\n");
        if (!client->GetRegistered()) {
            _server->RemoveClient(client);
        }
        return;
    }
    std::string oldNick = client->GetNick();
    client->SetNick(args[1]);

    if (client->GetRegistered()) {
        client->SendMessage(":" + oldNick + "!user@host NICK " + client->GetNick() + "\r\n");;
    }
}

void CommandManager::User(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 5) {
        client->SendMessage(":ircserver 461 USER :Not enough parameters\n");
        return;
    }
    if (client->GetRegistered()) {
        client->SendMessage(":ircserver 462 :You may not reregister\n");
        return;
    }
    std::string RealName = concatString(args, 4).erase(0, 1);
    client->SetUserName(args[1]);
    client->SetRealName(RealName);
    if (!client->getPassword()) {
        client->SendMessage(":ircserver 464 :Bad password\r\n");
        _server->RemoveClient(client);
        return;
    } else if (client->GetNick().empty()) {
        client->SendMessage(":ircserver 431 :No nickname given\r\n");
        _server->RemoveClient(client);
        return;
    }
    client->SetRegistered(true);
    client->SendMessage(":ircserver 001 " + client->GetNick() + " :Welcome to the Internet Relay Network\n");
}

void CommandManager::Ping(ClientIRC *client, std::vector<std::string> args) {
    std::string concat = "PONG " + concatString(args, 1) + "\r\n";
    client->SendMessage(concat);
    std::cout << concat << std::endl;
}

void CommandManager::Join(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 2) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " JOIN :Not enough parameters\n");
        return;
    }

    std::vector<std::string> channels = splitString(args[1], ",");
    for (iterator it = channels.begin(); it != channels.end(); ++it) {
        ChannelIRC *channel = this->_channelManager->GetChannel((*it));
        if (channel) {
            if (channel->GetMaxClients() != 0 && channel->GetClients().size() >= channel->GetMaxClients()) {
                client->SendMessage(":ircserver 471 ahamdoun " + (*it) + " :Cannot join channel (+l)\r\n");
                continue;
            }
            channel->AddClient(client);
        } else {
            this->_channelManager->CreateChannel((*it), client);
        }
    }
}

void CommandManager::Part(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 2) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " PART :Not enough parameters\n");
        return;
    }

    std::vector<std::string> channels = splitString(args[1], ",");
    for (iterator it = channels.begin(); it != channels.end(); ++it) {
        ChannelIRC *channel = this->_channelManager->GetChannel(*it);
        if (!channel) {
            std::cout << "Channel " << *it << " not found" << std::endl;
            client->SendMessage(":ircserver 403 " + client->GetNick() + " " + *it + " :No such channel\r\n");
            continue;
        }

        std::string reason = ":no reason";
        if (args.size() > 2) {
            reason = concatString(args, 2);
        }
        channel->SendMessage(":" + client->GetNick() + "!user@host PART " + *it + " " + reason + "\r\n", NULL);
        channel->RemoveClient(client);
    }
}

void CommandManager::PrivMSG(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 3) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " PRIVMSG :Not enough parameters\n");
        return;
    }

    std::string message = concatString(args, 2);
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    ClientIRC *clientT = this->_server->GetClientByNick(args[1]);
    if (!channel && !clientT) {
        client->SendMessage(":ircserver 403 " + client->GetNick() + " " + args[1] + " :No such channel\r\n");
        return;
    }
    if (channel) {
        if (!channel->HasClient(client)) {
            client->SendMessage(":ircserver 404 " + client->GetNick() + " " + args[1] + " :You are not in the chanel\r\n");
            return;
        }
        if (channel->GetCantVoice(client->GetNick())) {
            return;
        }
        std::cout << "Channel " << args[1] << " found send message " << message << std::endl;
        channel->SendMessage(":" + client->GetNick() + " PRIVMSG " + args[1] + " " + message + "\r\n", client);
    } else {
        clientT->SendMessage(":" + client->GetNick() + " PRIVMSG " + args[1] + " " + message + "\r\n");
    }
}

void CommandManager::List(ClientIRC *client, std::vector<std::string> args) {
    client->SendMessage(":ircserver 321 " + client->GetNick() + " :Channel :Users Name\r\n" );
    std::map<std::string, ChannelIRC *> chan = _channelManager->GetChannels();
    if (args.size() < 2)
    {
        std::stringstream ss;
        std::string count;
        for (std::map<std::string, ChannelIRC *>::iterator it = chan.begin();it != chan.end();it++)
        {
            ChannelIRC *channel = this->_channelManager->GetChannel(it->first);
            if (!channel) continue;
            ss << channel->getClientsCount();
            ss >> count;
            ss.clear();
            client->SendMessage(":ircserver 322 " + client->GetNick() + " "  + channel->GetName() + " " + count + ":" +  channel->GetTopic() + "Channel list - A channel\r\n" );
            std::cout << "CHANNEL |" << it->first << "|" << std::endl;
        }
    }
    else
    {
        for (iterator ite = args.begin();ite != args.end();ite++)
        {
            std::stringstream ss;
            std::string count;
            for (std::map<std::string, ChannelIRC *>::iterator it = chan.begin();it != chan.end();it++)
            {
                ChannelIRC *channel = this->_channelManager->GetChannel(it->first);
                if (!channel) continue;
                ss << channel->getClientsCount();
                ss >> count;
                ss.clear();
                std::string name = *ite;
                std::string comp = it->first;
                name = "#"+name;
                if (comp == name)
                {
                    client->SendMessage(":ircserver 322 " + client->GetNick() + " "  + channel->GetName() +
                    " " + count + ":" +  channel->GetTopic() + "Channel list - A channel\r\n" );
                    std::cout <<  "name == " << comp<< std::endl;
                }
            }
        }
    }
    client->SendMessage(":ircserver 323 " + client->GetNick() + " :End of /LIST\r\n");
}

void CommandManager::Topic(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 3) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " TOPIC :Not enough parameters\n");
        return;
    }

    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    if(!channel) {
        client->SendMessage(":ircserver 403 " + client->GetNick() + " " + args[1] + " :No such channel\r\n");
        return;
    }
    if (!channel->HasClient(client)) {
        client->SendMessage(":ircserver 442 " + client->GetNick() + " " + args[1] + " :You're not on that channel\r\n");
        return;
    }
    if (channel->GetOwner() != client) {
        client->SendMessage(":ircserver 482 " + client->GetNick() + " " + args[1] + " :You're not channel operator\r\n");
        return;
    }
    
    std::string topic = concatString(args, 2).erase(0, 1);
    channel->SetTopic(topic);
    channel->SendMessage(":" + client->GetNick() + " TOPIC " + args[1] + " " + topic + "\r\n", client);
}

void CommandManager::Restart(ClientIRC *client, std::vector<std::string> args) {
    (void)args;
    if (!client->GetOperator()) {
        client->SendMessage(":ircserver 481 " + client->GetNick() + " :Permission Denied- You're not an IRC operator\r\n");
        return;
    }
    int port = this->_server->getPort();
    std::string pwd = this->_server->getPassword();

    std::vector<ClientIRC *> clients = this->_server->getClients();
    for (std::vector<ClientIRC *>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if ((*it)->GetKilled()) continue;
        (*it)->SendMessage(":ircserver 421 " + (*it)->GetNick() + " :Server is restarting\r\n");
    }
    
    this->_server->Close();
    startServer(port, pwd);
}

void CommandManager::Kill(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 2) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " PRIVMSG :Not enough parameters\n");
        return;
    }
    ClientIRC *clientToKill = this->_server->GetClientByNick(args[1]);
    if (clientToKill) {
        clientToKill->SendMessage(":ircserver 421 " + clientToKill->GetNick() + " :You have been killed\r\n");
        this->_server->RemoveClient(clientToKill);
    } else {
        client->SendMessage(":ircserver 401 " + client->GetNick() + " " + args[1] + " :No such nick\r\n");
    }
}

void CommandManager::Quit(ClientIRC *client, std::vector<std::string> args) {
    std::string reason = ":No reason";
    if (args.size() > 1) {
        reason = concatString(args, 1);
    }

    std::map<std::string, ChannelIRC *> channels = _channelManager->GetChannels();
    for (std::map<std::string, ChannelIRC *>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->second && it->second->HasClient(client)) {
            it->second->SendMessage(":" + client->GetNick() + "!user@host QUIT " + reason + "\r\n", client);
            it->second->RemoveClient(client);
        }
    }
    _server->RemoveClient(client);

    std::cout << "Client disconnected with a QUIT" << std::endl;
}

void CommandManager::Mode(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 3) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " MODE :Not enough parameters\n");
        return;
    }
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);

    if (!channel) {
        client->SendMessage(":ircserver 403 " + client->GetNick() + " " + args[1] + " :No such channel\r\n");
        return;
    } else if (channel->GetOwner() != client) {
        client->SendMessage(":ircserver 482 " + client->GetNick() + " " + args[1] + " :You're not channel operator\r\n");
        return;
    }

    std::string type = args[2].substr(0, 1);
    args[2] = args[2].substr(1);

    std::vector<std::string> modes;
    for (std::string::iterator it = args[2].begin(); it != args[2].end(); ++it) {
        modes.push_back(std::string(1, *it));
    }

     for (iterator it = modes.begin(); it != modes.end(); ++it) {
        if (*it == "l") {
            if (args[3].empty()) {
                client->SendMessage(":ircserver 461 " + client->GetNick() + " MODE :Not enough parameters\r\n");
                return;
            }
            channel->SetMaxClients(atoi(args[3].c_str()));
            channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " +l " + args[3] + "\r\n", NULL);
        } else if (*it == "v") {
            if (args[3].empty()) {
                client->SendMessage(":ircserver 461 " + client->GetNick() + " MODE :Not enough parameters\r\n");
                return;
            }

            ClientIRC *clientToVoice = this->_server->GetClientByNick(args[3]);
            if (!clientToVoice) {
                client->SendMessage(":ircserver 401 " + client->GetNick() + " " + args[3] + " :No such nick\r\n");
                return;
            }

            if (type == "+") {
                channel->SetCantVoice(clientToVoice->GetNick(), true);
                channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " +v " + clientToVoice->GetNick() + "\r\n", client);
            } else if (type == "-") {
                channel->SetCantVoice(clientToVoice->GetNick(), false);
                channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " -v " + clientToVoice->GetNick() + "\r\n", client);
            }
        } else if (*it == "m") {
            if (type == "+") {
                channel->SetModerated(true);
                channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " +m\r\n", NULL);
            } else if (type == "-") {
                channel->SetModerated(false);
                channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " -m\r\n", NULL);
            }
        }
    }
}

void CommandManager::Oper(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 3) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " OPER :Not enough parameters\n");
        return;
    }

    if (args[1] == "admin" && args[2] == "1233") {
        client->SetOperator(true);
        client->SendMessage(":ircserver 381 " + client->GetNick() + " :You are now an IRC operator\r\n");
    } else {
        client->SendMessage(":ircserver 464 " + client->GetNick() + " :Password incorrect\r\n");
    }
}

void CommandManager::Kick(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 3) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " KICK :Not enough parameters\r\n");
        return;
    }
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    if (!channel) {
        client->SendMessage(":ircserver 403 " + client->GetNick() + " " + args[1] + " :No such channel\r\n");
        return;
    }

    if (channel->GetOwner() != client) {
        client->SendMessage(":ircserver 482 " + client->GetNick() + " " + args[1] + " :You're not channel operator\r\n");
        return;
    }

    ClientIRC *clientToKick = this->_server->GetClientByNick(args[2]);
    if (!clientToKick) {
        client->SendMessage(":ircserver 401 " + client->GetNick() + " " + args[2] + " :No such nick\r\n");
        return;
    }
    if (!channel->HasClient(clientToKick)) {
        client->SendMessage(":ircserver 441 " + client->GetNick() + " " + clientToKick->GetNick() + " " + channel->GetName() + " :They aren't on that channel\r\n");
        return;
    }

    std::string reason = ":No reason";   
    if (args.size() > 3 && args[3].length() > 1) {
        reason = concatString(args, 3);
    }

    clientToKick->SendMessage(":" + client->GetNick() + "!user@host NOTICE " + clientToKick->GetNick() + " :You have been kicked from " + channel->GetName() + " by " + client->GetNick() + " (" + reason.substr(1) + ")\r\n");
    channel->SendMessage(":" + client->GetNick() + "!user@host KICK " + channel->GetName() + " " + clientToKick->GetNick() + " " + reason + "\r\n", NULL);
    channel->RemoveClient(clientToKick);
}