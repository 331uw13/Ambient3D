#include <cstdio>
#include <string>

#include "network.hpp"


            
void AM::Network::close(asio::io_context& io_context) {
    printf("Closing network connection...\n");
    io_context.stop();
    m_event_handler_th.join();
    m_connected = false;
}
            

void AM::Network::m_do_read_tcp() {
    
    m_tcp_socket.async_read_some(asio::buffer(m_tcprecv_data, TCP_PROTO::MAX_PACKET_SIZE),
            [this](std::error_code ec, std::size_t size) {
                if(!ec) {
                    
                    printf("\033[33m<-\033[0m \"%s\" (%li)\n", m_tcprecv_data, size);
                    m_do_write_tcp();
                }
                else {
                    printf("[read](%i): %s\n", ec.value(), ec.message().c_str());
                }
            });

}
            
void AM::Network::write_message(const std::string& message) {
    m_tcp_write_buffer.insert(m_tcp_write_buffer.begin(), message);
    m_do_write_tcp();
}

void AM::Network::m_do_write_tcp() {
    
    if(m_tcp_write_buffer.empty()) {
        return;
    }

    std::string& msg = m_tcp_write_buffer.back();
    asio::async_write(m_tcp_socket, asio::buffer(msg.c_str(), msg.size()),
            [this, msg](std::error_code ec, std::size_t /*size*/) {
                if(!ec) {
                    printf("\033[32m->\033[0m \"%s\"\n", msg.c_str());
                    m_do_read_tcp();
                }
                else {
                    printf("[write](%i): %s\n", ec.value(), ec.message().c_str());
                }
            });

    m_tcp_write_buffer.pop_back();
}


