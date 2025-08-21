#include <cstdio>

#include "server.hpp"




int main() {
    
    printf("Ambient3D - Server\n");

    asio::io_context io_context;

    AM::Server server(io_context, 34480);
    server.start(io_context);



    return 0;
}

