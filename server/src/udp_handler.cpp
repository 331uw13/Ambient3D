#include <cstdio>

#include "udp_handler.hpp"


void AM::UDP_handler::m_do_read() {

    memset(m_data, 0, UDP_PROTO::MAX_PACKET_SIZE);
    m_socket.async_receive_from(
            asio::buffer(m_data, UDP_PROTO::MAX_PACKET_SIZE), m_sender_endpoint,
            [this](std::error_code ec, std::size_t size) {
                if(ec) {
                    printf("[read_udp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }

                printf("\033[33m (UDP) <-\033[0m \"%s\"\n", m_data);
                m_do_write();
            });
}

void AM::UDP_handler::m_do_write() {

    std::string msg = "Hello client. (This is UDP protocol)";

    m_socket.async_send_to(
            asio::buffer(msg.c_str(), msg.size()), m_sender_endpoint,
            [this, msg](std::error_code ec, std::size_t size) {
                if(ec) {
                    printf("[write_udp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }

                printf("\033[32m (UDP) ->\033[0m \"%s\"\n", msg.c_str());
                m_do_read();
            });


}




