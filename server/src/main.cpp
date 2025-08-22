#include <cstdio>

#include "server.hpp"




int main() {
    
    printf("Ambient3D - Server\n");

    asio::io_context io_context;

    const uint16_t tcp_port = 34480;
    const uint16_t udp_port = 34485;

    printf("Listening on port (tcp = %i), (udp = %i)\n",
            tcp_port, udp_port);

    AM::Server server(io_context, tcp_port, udp_port);
    server.start(io_context);



    return 0;
}

