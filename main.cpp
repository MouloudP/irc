#include "ServerIRC.hpp"

bool keepRunning = true;

void signalKill(int sig) {
    (void) sig;
    std::cout << "GOOD BYE" << std::endl;
    keepRunning = false;
}

void startServer(int port, std::string password) {
    ServerIRC server(port, password);
    server.Run();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    signal(SIGINT, signalKill);
    startServer(atoi(argv[1]), argv[2]);

    return 0;
}