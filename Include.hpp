#ifndef INCLUDE_HPP
# define INCLUDE_HPP
# include <iostream>
# include <cstdlib>
# include <cstring>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <poll.h>
# include <vector>
# include <map>
# include <signal.h>
# include <fcntl.h>

std::vector<std::string> splitString(std::string str, std::string delimiter);
#endif