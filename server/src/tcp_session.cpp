#include <cstdio>

#include "tcp_session.hpp"
#include "server.hpp"

// From '../../shared'
#include "packet_ids.hpp"
#include "packet_parser.hpp"


AM::TCP_session::TCP_session(tcp::socket socket, AM::Server* server) 
    : m_socket(std::move(socket)), m_server(server)
{
    this->uuid.generate();
}
            

void AM::TCP_session::m_handle_received_packet(size_t sizeb) {
    AM::PacketID packet_id = AM::parse_network_packet(m_data, sizeb);
    if(packet_id == AM::PacketID::NONE) {
        return;
    }

    switch(packet_id) {
        case AM::PacketID::CHAT_MESSAGE:
            printf("[CHAT]: %s\n", m_data);
            m_server->broadcast_tcp(m_data);
            break;

        // ...
    }
}

void AM::TCP_session::m_do_read() {
    
    memset(m_data, 0, AM::MAX_PACKET_SIZE);

    const std::shared_ptr<TCP_session>& self(shared_from_this());
    m_socket.async_read_some(asio::buffer(m_data, AM::MAX_PACKET_SIZE),
            [this, self](std::error_code ec, std::size_t size) {
                if(ec) {
                    printf("[read_tcp](%i): %s\n", ec.value(), ec.message().c_str());
                    m_server->remove_client(std::move(self));
                    return;
                }
                
                m_handle_received_packet(size);
                m_do_read();
                //this->write("");
            });

}


void AM::TCP_session::write(AM::PacketID packet_id, const std::string& msg) {

    /*
    char id_buffer[4] = { 0 };
    memmove(id_buffer, &packet_id, sizeof(AM::PacketID));
    */

    if(msg.size() >= (AM::MAX_PACKET_SIZE-sizeof(AM::PacketID))) {
        fprintf(stderr, "ERROR! Too much data for TCP packet to write.\n");
        return;
    }


    memset(m_packet_data, 0, 
            (m_packet_size < AM::MAX_PACKET_SIZE) ? m_packet_size : AM::MAX_PACKET_SIZE);
    m_packet_size = 0;

    // Set the packet id to be first 4 bytes.
    memmove(m_packet_data, &packet_id, sizeof(AM::PacketID));
    m_packet_size += sizeof(AM::PacketID);
    

    // Move rest of the message.
    memmove(m_packet_data + m_packet_size, &msg[0], msg.size());
    m_packet_size += msg.size();


    auto self(shared_from_this());
    asio::async_write(m_socket, asio::buffer(m_packet_data, m_packet_size),
            [this, self, msg](std::error_code ec, std::size_t /*size*/) {
                if(ec) {
                    printf("[write_tcp](%i): %s\n", ec.value(), ec.message().c_str());
                    m_server->remove_client(std::move(self));
                    return;
                }
                //printf("\033[32m (TCP) ->\033[0m \"%s\"\n", msg.c_str());
                //m_do_read();
            });
}


