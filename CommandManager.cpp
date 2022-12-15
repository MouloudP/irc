#include "CommandManager.hpp"

CommandManager::CommandManager(ChannelManager* _channelManager, ServerIRC *srv): _channelManager(_channelManager), _server(srv) {}
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
    } else if (commandName == "TOPIC"){
        this->Topic(client, args);
    }
        //client->SendMessage(":mouloud 421 ahamdoun :Unknown command\n");
}


 void CommandManager::Pass(ClientIRC *client, std::vector<std::string> args) {
    std::cout << "PASS ==" << this->_server->getPassword() << std::endl;
       std::cout << "args == " << args[1] << std::endl;
    if (args[1] == this->_server->getPassword()) {
        //client->SetPassword(args[1]);
        client->SendMessage(":mouloud 001 " + client->GetUserName() + " : Welcome to the Internet Relay Network " + client->GetUserName() + "\r\n");
    }
    else if (args[1].empty()) {
         client->SendMessage(":mouloud 461" + client->GetUserName() + args[0] + ": Need more param\n");
    } else {
        client->SendMessage(":mouloud 464 " + client->GetUserName() + " : Password incorrect\r\n");
        std::cout << "fd ==== " << client->GetFd() << std::endl;
        close(client->GetFd());
    }

}   

void CommandManager::Nick(ClientIRC *client, std::vector<std::string> args) {
    //std::cout << "NICK : " << args[1] << std::endl;

    std::cout << " nbfffff -=== " << buf\\ << std::endl;
    if (args[1].empty()) {
        client->SendMessage(":mouloud 431 ahamdoun" + client->GetUserName() + ": no nickname\n");
    } else {
        //client->SendMessage(":mouloud 433 ahamdoun :ahamdoun is already in use\n");
        client->SetNick(args[1]);
    }
    std::cout << "_________________________" << std::endl;
    std::cout << client->GetNick() << std::endl;
    std::cout << "_________________________" << std::endl;
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
    client->SetRegistered(true);
    client->SendMessage(":mouloud 001 ahamdoun :Welcome to the Internet Relay Network\n");
}

void CommandManager::Ping(ClientIRC *client, std::vector<std::string> args) {
    std::string concat = "PONG " + concatString(args, 1) + "\r\n";
    client->SendMessage(concat);
    std::cout << concat << std::endl;
}

void CommandManager::Join(ClientIRC *client, std::vector<std::string> args) {
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    if (channel) {
        channel->AddClient(client);
    } else {
        this->_channelManager->CreateChannel(args[1], client);
        //std::cout << "Channel " << args[1] << " created" << std::endl;
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
        std::cout << "PART " << *it << std::endl;
        channel->RemoveClient(client);
    }
}

void CommandManager::PrivMSG(ClientIRC *client, std::vector<std::string> args) {
    std::string message = concatString(args, 2);
    std::cout << "PRIVMSG " << args[1] << " : " << message << std::endl;
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    if (channel) {
        std::cout << "Channel " << args[1] << " found send message " << message << std::endl;
        if (channel->HasClient(client))
            channel->SendMessage(":" + client->GetUserName() + " PRIVMSG " + args[1] + " " + message + "\r\n", client);
        else
            client->SendMessage(":mouloud 404 " + client->GetUserName() + " " + args[1] + " :You are not in the chanel\r\n");
    } else {
        std::cout << "Channel " << args[1] << " not found" << std::endl;
        client->SendMessage(":mouloud 403 ahamdoun " + args[1] + " :No such channel\r\n");
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
            std::cout << "count == " << count << std::endl;
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
    client->SendMessage(": mouloud 323 " + client->GetUserName() + " :End of /LIST\r\n");
}

void CommandManager::Topic(ClientIRC *client, std::vector<std::string> args) {
    ChannelIRC *channel = this->_channelManager->GetChannel(args[1]);
    if (args[1].empty())
        client->SendMessage(":mouloud 461" + client->GetUserName() + args[0] + ":Need more param\n");

        //return  client->SendMessage(":mouloud 331" + client->GetUserName() + args[0] + ":No topic is set\n");
    //else if(!channel->HasClient(client))// a verifier
      //  client->SendMessage(":mouloud 404 " + client->GetUserName() + " " + args[1] + " :You're not in the channel\r\n"); 
}