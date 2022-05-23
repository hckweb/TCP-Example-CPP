#include "TCPClient.h"

#include <iostream>

constexpr char json_data[] = "{\"command\": \"read data\"}";

int main()
{
    Client client("127.0.0.1", 3642);

    client.send(std::string(json_data));

    std::cout << client.read() << "\n";

    while (1)
        ;

    return 0;
}
