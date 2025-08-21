#include <iostream>

#include "tcp_session.hpp"
#include "networking_agreements.hpp"




void AM::TCP_session::m_do_read() {

    auto self(shared_from_this());
    m_socket.async_read_some(asio::buffer(m_data, TCP_PROTO::MAX_PACKET_SIZE),
            [this, self](std::error_code ec, std::size_t size) {
                if(!ec) {
                    printf("\033[33m<-\033[0m \"%s\"\n", m_data);
                    m_do_write();
                }
                else {
                    std::cout << "[read]("<< ec.value() <<"): " << ec.message() << std::endl;
                }
            });

}


void AM::TCP_session::m_do_write() {

    std::string msg = "Hello client.";

    auto self(shared_from_this());
    asio::async_write(m_socket, asio::buffer(msg.c_str(), msg.size()),
            [this, self, msg](std::error_code ec, std::size_t /*size*/) {
                if(!ec) {
                    printf("\033[32m->\033[0m \"%s\"\n", msg.c_str());
                    m_do_read();
                }
                else {
                    std::cout << "[write]("<< ec.value() <<"): " << ec.message() << std::endl;
                }
            });

}






