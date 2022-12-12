#include "irc_server.hpp"

ServerIRC::ServerIRC() {
    std::cout << "ServerIRC::ServerIRC()" << std::endl;
}

ServerIRC::ServerIRC(int port): port(port) {
    std::cout << "ServerIRC::ServerIRC(int)" << std::endl;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    int yes = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    channelManager = new ChannelManager();
}

ServerIRC::~ServerIRC() {
    close(sockfd);

    std::cout << "ServerIRC::~ServerIRC()" << std::endl;
}

ClientIRC *ServerIRC::CreateClient() {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    std::cout << sockfd << std::endl;
    int clientfd = accept(sockfd, (struct sockaddr *)&client, &client_len);

    if (clientfd == -1) {
        perror("accept");
        exit(1);
    }

    FD_SET(clientfd, &current_sockets);

    ClientIRC *client_irc = new ClientIRC(clientfd);
    Clients.push_back(client_irc);

    /*for (auto it = Clients.begin(); it != Clients.end(); ++it) {
        std::cout << (*it)->GetFd() << std::endl;
    }*/

    //std::cout << Clients.size() << std::endl;

    /*Protocol connection*/
    std::cout << "[" << client_irc->GetFd() << "]" << "Client connected" << std::endl;

    if (fcntl(client_irc->GetFd(), F_SETFL, fcntl(client_irc->GetFd(), F_GETFL) | O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(1);
    }
    return client_irc;
}

void ServerIRC::Run() {
    FD_ZERO(&current_sockets);
    FD_SET(sockfd, &current_sockets);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    while (true) {
        ready_sockets = current_sockets;
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, &timeout) < 0) {
            perror("select");
            exit(1);
        }

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &ready_sockets)) {
                if (i == sockfd) {
                    ClientIRC *test = this->CreateClient();
                } else {
                    FD_CLR(i, &current_sockets);
                }
            }
        }

        // Iteration on Clients
        for (auto it = Clients.begin(); it != Clients.end(); ++it) {
           //Non blocking socket

            std::string request = "";
            std::string dataRead = "";
            char buffer[1024];

            int lenght = recv((*it)->GetFd(), buffer, sizeof(buffer), 0);
            if (lenght > 0) {
                buffer[lenght] = '\0';
                //std::cout << "\\BUFFER = " << buffer << std::endl;
                fflush(stdout);
                request = buffer;
            }
            if (!request.empty()) {

                MesssageReceived((*it), request);
            }
            buffer[0] = '\0';
        }
    }
}

void ServerIRC::Close() {
    close(sockfd);
    for (auto it = Clients.begin(); it != Clients.end(); ++it) {
        close((*it)->GetFd());
        delete *it;
    }
}

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

void ServerIRC::MesssageReceived(ClientIRC *client, std::string message) {
    //std::cout << "RECEIVE FROM [" << client->GetFd() << "] : " << message << std::endl;

    std::vector<std::string> messages = splitString(message, "\n");
    for (auto it = messages.begin(); it != messages.end(); ++it) {
        this->ExecuteCommand(client, *it);
    }
}

void ServerIRC::ExecuteCommand(ClientIRC *client, std::string command) {
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
    if (args[0] == "PASS")
    {
        if (args[1].empty())
              client->SendMessage(":mouloud 461" + client->GetUserName() + args[0] + ":Need more param\n");
        if(!client->GetRegistered())
            client->SendMessage(":mouloud 462" + client->GetUserName()  + ":You're already register \n");

    }
    else if (args[0] == "NICK") {

        if (args[1].empty())
            client->SendMessage(":mouloud 431" + client->GetUserName() + ":	Returned when a nickname parameter expected for a command isn't found");
        if (args[1] == "_") {
            client->SendMessage(":mouloud 432 ahamdoun :Erroneous nickname\n");
        //client->SendMessage(":mouloud 433 ahamdoun :ahamdoun is already in use\n");
        client->SetNick(args[1]);
        std::cout << "_________________________" << std::endl;
        std::cout << client->GetNick() << std::endl;
        std::cout << "_________________________" << std::endl;

        /*The nick is already take*/
        
    } else if (args[0] == "USER") {
        std::cout << "USER" << std::endl;
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
    } else if (args[0] == "PING") {
        std::string concat = "PONG " + concatString(args, 1) + "\r\n";
        client->SendMessage(concat);
        std::cout << concat << std::endl;
    } else if (args[0] == "JOIN") {

        if (args[1].empty())
              client->SendMessage(":mouloud 461" + client->GetUserName() + args[0] + ":Need more param\n");

        ChannelIRC *channel = this->channelManager->GetChannel(args[1]);
        if (channel) {
            channel->AddClient(client);
        } else {
            this->channelManager->CreateChannel(args[1], client);
            //std::cout << "Channel " << args[1] << " created" << std::endl;
        }
    } else if (args[0] == "PART") {
         if (args[1].empty())
              client->SendMessage(":mouloud 461" + client->GetUserName() + args[0] + ":Need more param\n");
        std::vector<std::string> channels = splitString(args[1], ",");
        for (auto it = channels.begin(); it != channels.end(); ++it) {
            ChannelIRC *channel = this->channelManager->GetChannel(*it);
            if (!channel) {
                std::cout << "Channel " << *it << " not found" << std::endl;
                client->SendMessage(":mouloud 403 ahamdoun " + *it + " :No such channel\r\n");
                continue;
            }
            else if (!channel->HasClient(client))
                 client->SendMessage(":mouloud 442" + client->GetUserName() + " " + channel->GetName() + " :You're not a member of the channel\r\n");
            else
                /* sinon remove le client du channel */
            std::cout << "PART " << *it << std::endl;
            channel->RemoveClient(client);
        }
    } else if (args[0] == "PRIVMSG") {
        std::string message = concatString(args, 2);
        std::cout << "PRIVMSG " << args[1] << " : " << message << std::endl;
        ChannelIRC *channel = this->channelManager->GetChannel(args[1]);
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
    } else if (args[0] == "LIST"){

        client->SendMessage(":mouloud 321 " + client->GetUserName() + " :Channel :Users Name\r\n" );
        std::map<std::string, ChannelIRC *> chan = channelManager->GetChannels();
        if (args[1].empty())
        {
            std::stringstream ss;
            std::string count;
            for (auto it = chan.begin();it != chan.end();it++)
            {

                ChannelIRC *channel = this->channelManager->GetChannel(it->first);
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
                    ChannelIRC *channel = this->channelManager->GetChannel(it->first);
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
    }else if (args[0] == "TOPIC")
    {
        ChannelIRC *chan; 
         if (args[1].empty())
            client->SendMessage(":mouloud 461" + client->GetUserName() + args[0] + ":Need more param\n");
        if(!chan->HasClient(client))// a verifier
            client->SendMessage(":mouloud 404 " + client->GetUserName() + " " + args[1] + " :You're not in the channel\r\n");

    }else if (args[0] == "QUIT")
    {

    }
 }         
    