#include <cstdio>

#include "udp_handler.hpp"
#include "packet_ids.hpp"
#include "packet_parser.hpp"


void AM::UDP_handler::m_handle_received_packet(size_t sizeb) {
    AM::PacketID packet_id = AM::parse_network_packet(m_data, sizeb);
    if(packet_id == AM::PacketID::NONE) {
        return;
    }


    //printf("(UDP) PacketID: %i\n", packet_id);
}

void AM::UDP_handler::m_do_read() {

    memset(m_data, 0, AM::MAX_PACKET_SIZE);
    m_socket.async_receive_from(
            asio::buffer(m_data, AM::MAX_PACKET_SIZE), m_sender_endpoint,
            [this](std::error_code ec, std::size_t size) {
                if(ec) {
                    printf("[read_udp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }

                m_handle_received_packet(size);                
                m_do_read();
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
                //printf("\033[32m (UDP) ->\033[0m \"%s\"\n", msg.c_str());
                //m_do_read();
            });
}




