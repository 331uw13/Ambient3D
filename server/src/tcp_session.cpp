#include <cstdio>

#include "tcp_session.hpp"
#include "networking_agreements.hpp"

#include "server.hpp"

AM::TCP_session::TCP_session(tcp::socket socket, AM::Server* server) 
    : m_socket(std::move(socket)), m_server(server)
{
    this->uuid.generate();
}

void AM::TCP_session::m_do_read() {
    
    memset(m_data, 0, TCP_PROTO::MAX_PACKET_SIZE);

    const std::shared_ptr<TCP_session>& self(shared_from_this());
    m_socket.async_read_some(asio::buffer(m_data, TCP_PROTO::MAX_PACKET_SIZE),
            [this, self](std::error_code ec, std::size_t size) {
                if(ec) {
                    printf("[read_tcp](%i): %s\n", ec.value(), ec.message().c_str());
                    m_server->remove_client(std::move(self));
                    return;
                }
                printf("\033[33m (TCP) <-\033[0m \"%s\"\n", m_data);
                
                this->write("got your msg");
            });

}


void AM::TCP_session::write(const std::string& msg) {

    auto self(shared_from_this());
    asio::async_write(m_socket, asio::buffer(msg.c_str(), msg.size()),
            [this, self, msg](std::error_code ec, std::size_t /*size*/) {
                if(ec) {
                    printf("[write_tcp](%i): %s\n", ec.value(), ec.message().c_str());
                    m_server->remove_client(std::move(self));
                    return;
                }
                printf("\033[32m (TCP) ->\033[0m \"%s\"\n", msg.c_str());
                
                m_do_read();
            });
}


