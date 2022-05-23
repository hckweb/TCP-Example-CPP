#include "TCPServer.h"

#include <string>

std::string callback(std::string)
{
    return std::string("TEST");
}

int main()
{
    TCPServer a(3642, callback);

    return 0;
}