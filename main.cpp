#include "irc_server.hpp"

//ServerIRC server;

/*void signalKill(int sig) {
    server.Close();
    std::cout << "TURNING OFF" << std::endl;
    exit(0);
}*/

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    //signal(SIGINT, signalKill);
    ServerIRC server(atoi(argv[1]));
    server.Run();
    

    return 0;
}