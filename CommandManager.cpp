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
    for (int i = start; i < str.size(); i++) {
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
    } else if (commandName == "PASS") {
        this->Pass(client, args);
    } else if (commandName == "USER") {
        this->User(client, args);
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
    } else if (commandName == "restart") {
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
        client->SendMessage(":mouloud 421 ahamdoun " + commandName +  ":Unknown command\n");
    }
}



void CommandManager::Pass(ClientIRC *client, std::vector<std::string> args) {
    if (args[1] == this->_server->getPassword()) {
        client->setPassword(true);
        std::cout << "GOOD PASSWORD" << std::endl;
        client->SendMessage(":mouloud 001 " + client->GetUserName() + " : Welcome to the Internet Relay Network " + client->GetUserName() + "\r\n");
    }
    else if (args[1].empty()) {
         client->SendMessage(":mouloud 461" + client->GetUserName() + args[0] + ": Need more param\n");
         return ;
    } else {
        client->SendMessage(":mouloud 464 " + client->GetUserName() + " : Password incorrect\r\n");
        _server->RemoveClient(client);
    }
}   

void CommandManager::Nick(ClientIRC *client, std::vector<std::string> args) {
    if (args[1].empty()) {
        client->SendMessage(":mouloud 431 ahamdoun" + client->GetNick() + ": no nickname\n");
        return;
    }

    ClientIRC *client2 = this->_server->GetClientByNick(args[1]);
    if (client2 != NULL) {
        client->SendMessage(":mouloud 433 ahamdoun" + client->GetNick() + ": nickname already in use\n");
        if (!client->GetRegistered()) {
            _server->RemoveClient(client);
        }
        return;
    }

    ClientIRC *client2 = this->_server->GetClientByNick(args[1]);
    if (client2 != NULL) {
        client->SendMessage(":mouloud 433 ahamdoun" + client->GetNick() + ": nickname already in use\n");
        if (!client->GetRegistered()) {
            _server->RemoveClient(client);
        }
        return;
    }
    std::string oldNick = client->GetNick();
    client->SetNick(args[1]);
    std::cout << "_________________________" << std::endl;
    std::cout << client->GetNick() << std::endl;
    std::cout << "_________________________" << std::endl;

    if (client->GetRegistered()) {
        client->SendMessage(":" + oldNick + "!user@host NICK " + client->GetNick() + "\r\n");;
    }
}

void CommandManager::User(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() - 1 < 4) {
        client->SendMessage(":mouloud 461 ahamdoun USER :Not enough parameters\n");
        return;
    }
    if (client->GetRegistered()) {
        client->SendMessage(":mouloud 462 ahamdoun :You may not reregister\n");
        return;
    }
    std::string RealName = concatString(args, 4).erase(0, 1);
    std::cout << "RealName : " << RealName << std::endl;
    client->SetUserName(args[1]);
    client->SetRealName(RealName);
    if (client->getPassword() == true ){
        client->SetRegistered(true);
        client->SendMessage(":mouloud 001 ahamdoun :Welcome to the Internet Relay Network\n");
    }else
         _server->RemoveClient(client);

}

void CommandManager::Ping(ClientIRC *client, std::vector<std::string> args) {
    std::string concat = "PONG " + concatString(args, 1) + "\r\n";
    client->SendMessage(concat);
    std::cout << concat << std::endl;
}

void CommandManager::Join(ClientIRC *client, std::vector<std::string> args) {
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    if (args[1].empty())
        client->SendMessage(":mouloud 431 ahamdoun" + client->GetUserName() + ": Need more param\n");
    if (channel) {
        if (channel->GetMaxClients() != 0 && channel->GetClients().size() >= channel->GetMaxClients()) {
            client->SendMessage(":mouloud 471 ahamdoun " + args[1] + " :Cannot join channel (+l)\r\n");
            return;
        }
        channel->AddClient(client);
    } else {
        this->_channelManager->CreateChannel(args[1], client);
    }
}

void CommandManager::Part(ClientIRC *client, std::vector<std::string> args) {
    std::vector<std::string> channels = splitString(args[1], ",");
    for (auto it = channels.begin(); it != channels.end(); ++it) {
        ChannelIRC *channel = this->_channelManager->GetChannel(*it);
        if (!channel) {
            std::cout << "Channel " << *it << " not found" << std::endl;
            client->SendMessage(":mouloud 403 ahamdoun " + *it + " :No such channel\r\n");
            continue;
        }

        std::string reason = ":no reason";
        if (args.size() > 2) {
            reason = concatString(args, 2);
        }
        channel->SendMessage(":" + client->GetNick() + "!user@host PART " + *it + " " + reason + "\r\n", client);
        client->SendMessage(":" + client->GetNick() + "!user@host KICK " + *it + " " + client->GetNick() +  " " + reason + "\r\n");
        channel->RemoveClient(client);
    }
}

void CommandManager::PrivMSG(ClientIRC *client, std::vector<std::string> args) {
    std::string message = concatString(args, 2);
    std::cout << "PRIVMSG " << args[1] << " : " << message << std::endl;
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    ClientIRC *clientT = this->_server->GetClientByNick(args[1]);
    if (!channel && !clientT) {
        client->SendMessage(":mouloud 403 " + client->GetNick() + " " + args[1] + " :No such channel\r\n");
        return;
    }
    if (channel) {
        if (!channel->HasClient(client)) {
            client->SendMessage(":mouloud 404 " + client->GetUserName() + " " + args[1] + " :You are not in the chanel\r\n");
            return;
        }
        if (channel->GetCantVoice(client->GetNick())) {
            //client->SendMessage(":mouloud 404 " + client->GetUserName() + " " + args[1] + " :Cannot send to channel\r\n");
            return;
        }
        std::cout << "Channel " << args[1] << " found send message " << message << std::endl;
        channel->SendMessage(":" + client->GetNick() + " PRIVMSG " + args[1] + " " + message + "\r\n", client);
    } else {
        clientT->SendMessage(":" + client->GetNick() + " PRIVMSG " + args[1] + " " + message + "\r\n");
    }
}

void CommandManager::List(ClientIRC *client, std::vector<std::string> args) {
    client->SendMessage(":mouloud 321 " + client->GetUserName() + " :Channel :Users Name\r\n" );
    std::map<std::string, ChannelIRC *> chan = _channelManager->GetChannels();
    if (args[1].empty())
    {
        std::stringstream ss;
        std::string count;
        for (auto it = chan.begin();it != chan.end();it++)
        {
            ChannelIRC *channel = this->_channelManager->GetChannel(it->first);
            ss << channel->getClientsCount();
            ss >> count;
            ss.clear();
            client->SendMessage(": mouloud 322 " + client->GetUserName() + " "  + channel->GetName() +
                " " + count + ":" +  channel->GetTopic() + "Channel list - A channel\r\n" );
            std::cout << "CHANNEL |" << it->first << "|" << std::endl;
        }
    }
    else
    {
        for (auto ite = args.begin();ite != args.end();ite++)
        {
            std::stringstream ss;
            std::string count;
            for (auto it = chan.begin();it != chan.end();it++)
            {
                ChannelIRC *channel = this->_channelManager->GetChannel(it->first);
                ss << channel->getClientsCount();
                ss >> count;
                ss.clear();
                std::string name = *ite;
                std::string comp = it->first;
                name = "#"+name;
                if (comp == name)
                {
                    client->SendMessage(": mouloud 322 " + client->GetUserName() + " "  + channel->GetName() +
                    " " + count + ":" +  channel->GetTopic() + "Channel list - A channel\r\n" );
                    std::cout <<  "name == " << comp<< std::endl;
                }
            }
        }
    }
    client->SendMessage(":mouloud 323 " + client->GetUserName() + " :End of /LIST\r\n");
}

void CommandManager::Topic(ClientIRC *client, std::vector<std::string> args) {
    if (args[1].empty() || args[2].empty())
        client->SendMessage(":mouloud 461" + client->GetNick() + args[0] + ":Need more param\n");

    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    if(!channel) {
        client->SendMessage(":mouloud 403 " + client->GetNick() + " " + args[1] + " :No such channel\r\n");
        return;
    }
    if (!channel->HasClient(client)) {
        client->SendMessage(":mouloud 442 " + client->GetNick() + " " + args[1] + " :You're not on that channel\r\n");
        return;
    }
    if (channel->GetOwner() != client) {
        client->SendMessage(":mouloud 482 " + client->GetNick() + " " + args[1] + " :You're not channel operator\r\n");
        return;
    }
    
    std::string topic = concatString(args, 2).erase(0, 1);
    channel->SetTopic(topic);
    channel->SendMessage(":" + client->GetNick() + " TOPIC " + args[1] + " " + topic + "\r\n", client);
}

void CommandManager::Restart(ClientIRC *client, std::vector<std::string> args) {
    int port = this->_server->getPort();
    std::string pwd = this->_server->getPassword();

    std::vector<ClientIRC *> clients = this->_server->getClients();
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        (*it)->SendMessage(":mouloud 421 " + (*it)->GetUserName() + " :Server is restarting\r\n");
    }
    
    this->_server->Close();
    startServer(port, pwd);
    delete this->_server;
}

void CommandManager::Kill(ClientIRC *client, std::vector<std::string> args) {
    ClientIRC *clientToKill = this->_server->GetClientByNick(args[1]);
    if (clientToKill) {
        clientToKill->SendMessage(":mouloud 421 " + clientToKill->GetUserName() + " :You have been killed\r\n");
        this->_server->RemoveClient(clientToKill);
    } else {
        client->SendMessage(":mouloud 401 " + client->GetUserName() + " " + args[1] + " :No such nick\r\n");
    }
}

void CommandManager::Quit(ClientIRC *client, std::vector<std::string> args) {
    //std::cout << "QUIT " << message << std::endl;
    /*client->SendMessage(":mouloud 421 " + client->GetUserName() + " :Server is restarting\r\n");
    this->_server->RemoveClient(client);*/

    std::string reason = ":No reason";
    if (args.size() > 1) {
        reason = concatString(args, 1);
    }

    std::map<std::string, ChannelIRC *> channels = _channelManager->GetChannels();
    for (auto it = channels.begin(); it != channels.end(); ++it) {
        if (it->second->HasClient(client)) {
            it->second->SendMessage(":" + client->GetNick() + "!user@host QUIT " + reason + "\r\n", client);
            it->second->RemoveClient(client);
        }
    }
    _server->RemoveClient(client);
}

void CommandManager::Mode(ClientIRC *client, std::vector<std::string> args) {
    
     if (!client->GetOperator()) {
        client->SendMessage(":mouloud 482 " + client->GetNick() + " :You're not an IRC operator\r\n");
        return;
    } 

    if (args.size() < 3) {
        client->SendMessage(":mouloud 461 " + client->GetNick() + " " + args[0] + " :Not enough parameters\r\n");
        return;
    }
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);

    if (!channel) {
        client->SendMessage(":mouloud 403 " + client->GetNick() + " " + args[1] + " :No such channel\r\n");
        return;
    }

    //+b, +l, +i, +v, +m

    std::string type = args[2].substr(0, 1);
    args[2] = args[2].substr(1);
    //split each char
    std::vector<std::string> modes;
    for (auto it = args[2].begin(); it != args[2].end(); ++it) {
        modes.push_back(std::string(1, *it));
    }

    for (auto it = modes.begin(); it != modes.end(); ++it) {
        if (*it == "l") {
            if (args[3].empty()) {
                client->SendMessage(":mouloud 461 " + client->GetNick() + " " + args[0] + " :Not enough parameters\r\n");
                return;
            }
            channel->SetMaxClients(std::stoi(args[3]));
            //client->SendMessage(":mouloud 324 " + client->GetNick() + " " + channel->GetName() + " +l " + args[3] + "\r\n");
            channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " +l " + args[3] + "\r\n", NULL);
        } else if (*it == "b") {
            if (args[3].empty()) {
                client->SendMessage(":mouloud 461 " + client->GetNick() + " " + args[0] + " :Not enough parameters\r\n");
                return;
            }
            channel->SetBanPatern(args[3]);
            //client->SendMessage(":mouloud 324 " + client->GetNick() + " " + channel->GetName() + " +b " + args[3] + "\r\n");
            channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " +b " + args[3] + "\r\n", NULL);
        } else if (*it == "v") {
            //if (!channel->GetModerated()) {
                //client->SendMessage(":mouloud 482 " + client->GetNick() + " " + channel->GetName() + " :Channel isn't moderated\r\n");
                //return;
            //}

            if (args[3].empty()) {
                client->SendMessage(":mouloud 461 " + client->GetNick() + " " + args[0] + " :Not enough parameters\r\n");
                return;
            }

            ClientIRC *clientToVoice = this->_server->GetClientByNick(args[3]);
            if (!clientToVoice) {
                client->SendMessage(":mouloud 401 " + client->GetNick() + " " + args[3] + " :No such nick\r\n");
                return;
            }

            if (type == "+") {
                channel->SetCantVoice(clientToVoice->GetNick(), true);
                channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " +v " + clientToVoice->GetNick() + "\r\n", client);
                //client->SendMessage(":mouloud 324 " + client->GetNick() + " " + channel->GetName() + " +v " + clientToVoice->GetNick() + "\r\n");
            } else if (type == "-") {
                channel->SetCantVoice(clientToVoice->GetNick(), false);
                channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " -v " + clientToVoice->GetNick() + "\r\n", client);
                //client->SendMessage(":mouloud 324 " + client->GetNick() + " " + channel->GetName() + " -v " + clientToVoice->GetNick() + "\r\n");
            }
        } else if (*it == "m") {
            if (type == "+") {
                channel->SetModerated(true);
                //client->SendMessage(":mouloud 324 " + client->GetNick() + " " + channel->GetName() + " +m\r\n");
                channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " +m\r\n", NULL);
            } else if (type == "-") {
                channel->SetModerated(false);
                //client->SendMessage(":mouloud 324 " + client->GetNick() + " " + channel->GetName() + " -m\r\n");
                channel->SendMessage(":" + client->GetNick() + "!user@host MODE " + channel->GetName() + " -m\r\n", NULL);
            }
        } /*else if (*it == "i") {
            if (type == "+") {
                channel->SetInviteOnly(true);
                client->SendMessage(":mouloud 324 " + client->GetNick() + " " + channel->GetName() + " +i\r\n");
            } else if (type == "-") {
                channel->SetInviteOnly(false);
                client->SendMessage(":mouloud 324 " + client->GetNick() + " " + channel->GetName() + " -i\r\n");
            }
        }*/
    }
}

void CommandManager::Oper(ClientIRC *client, std::vector<std::string> args) {
    if (args[1].empty() || args[2].empty()) {
        client->SendMessage(":mouloud 461 " + client->GetUserName() + " " + args[0] + " :Not enough parameters\r\n");
        return;
    }

    if (args[1] == "admin" && args[2] == "1233") {
        client->SetOperator(true);
        client->SendMessage(":mouloud 381 " + client->GetUserName() + " :You are now an IRC operator\r\n");
    } else {
        client->SendMessage(":mouloud 464 " + client->GetUserName() + " :Password incorrect\r\n");
    }
}

void CommandManager::Kick(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 3) {
        client->SendMessage(":mouloud 461 " + client->GetNick() + " " + args[0] + " :Not enough parameters\r\n");
        return;
    }
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    if (!channel) {
        client->SendMessage(":mouloud 403 " + client->GetNick() + " " + args[1] + " :No such channel\r\n");
        return;
    }

    if (channel->GetOwner() != client) {
        client->SendMessage(":mouloud 482 " + client->GetNick() + " " + args[1] + " :You're not channel operator\r\n");
        return;
    }

    ClientIRC *clientToKick = this->_server->GetClientByNick(args[2]);
    if (!clientToKick) {
        client->SendMessage(":mouloud 401 " + client->GetNick() + " " + args[2] + " :No such nick\r\n");
        return;
    }
    if (!channel->HasClient(clientToKick)) {
        client->SendMessage(":mouloud 441 " + client->GetNick() + " " + clientToKick->GetNick() + " " + channel->GetName() + " :They aren't on that channel\r\n");
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