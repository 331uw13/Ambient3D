#include <cstdio>
#include <string>
#include <iostream>

#include "network.hpp"


AM::Network::Network(asio::io_context& io_context, const NetConnectCFG& cfg)
    : m_tcp_socket(io_context),
      m_udp_socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), std::stoi(cfg.udp_port_client)))
{
    m_tcp_chatbox = cfg.tcp_chatbox;
    if(m_tcp_chatbox) {
        m_tcp_chatbox->push_message(GREEN, 
                TextFormat("Connecting to chat %s:%s (tcp)", cfg.host, cfg.tcp_port));
    }

    try {
        asio::ip::tcp::resolver tcp_resolver(io_context);
        asio::connect(m_tcp_socket, tcp_resolver.resolve(cfg.host, cfg.tcp_port));

        asio::ip::udp::resolver udp_resolver(io_context);
        m_udp_sender_endpoint = *udp_resolver.resolve(cfg.host, cfg.udp_port_server).begin();
    
        memset(m_tcprecv_data, 0, TCP_PROTO::MAX_PACKET_SIZE);
        memset(m_udprecv_data, 0, UDP_PROTO::MAX_PACKET_SIZE);

        m_connected = true;
        m_do_read_tcp();
        m_do_write_udp();

        // Start event handler.
        m_event_handler_th = std::thread([](asio::io_context& context){
                context.run();
                }, std::ref(io_context));
       
        if(m_tcp_chatbox) {
            m_tcp_chatbox->push_message(GREEN, "Connected!");
        }

    }
    catch(const std::exception& e) {
        fprintf(stderr, "%s: %s\n",
                __func__, e.what());
    }
}

void AM::Network::close(asio::io_context& io_context) {
    printf("Closing network connection...\n");
    io_context.stop();
    m_event_handler_th.join();
    m_connected = false;
}
 

void AM::Network::write_string(AM::NetProto proto, const std::string& data) {
    if(proto == AM::NetProto::TCP) {
        m_tcp_write_buffer.push_back(data);
        m_do_write_tcp();
        printf("%s (TCP)\n",__func__);
    }
    else
    if(proto == AM::NetProto::UDP) {
        printf("%s (UDP) '%s'\n",__func__,data.c_str());
        m_udp_write_buffer.push_back(data);
        m_do_write_udp();
    }
}           

// TCP ------------------------------------------------------

void AM::Network::m_do_read_tcp() {
    
    memset(m_tcprecv_data, 0, TCP_PROTO::MAX_PACKET_SIZE);

    m_tcp_socket.async_read_some(asio::buffer(m_tcprecv_data, TCP_PROTO::MAX_PACKET_SIZE),
            [this](std::error_code ec, std::size_t size) {
                if(ec) {
                    printf("[read_tcp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }

                if(m_tcp_chatbox) {
                    m_tcp_chatbox->push_message(WHITE, m_tcprecv_data);
                }
                printf("\033[33m (TCP) <-\033[0m \"%s\" (%li)\n", m_tcprecv_data, size);
                m_do_write_tcp();
            });

}


void AM::Network::m_do_write_tcp() {
    if(m_tcp_write_buffer.empty()) { 
        printf("%s: Nothing to write.\n",__func__);
        return;
    }
    
    asio::async_write(m_tcp_socket, 
            asio::buffer(m_tcp_write_buffer.front(), m_tcp_write_buffer.front().size()),
            [this](std::error_code ec, std::size_t /*size*/) {
                if(ec) {
                    printf("[write_tcp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }
                
                if(!m_tcp_write_buffer.empty()) {
                    m_tcp_write_buffer.pop_front();
                }
                m_do_read_tcp();
            });
}




// UDP ------------------------------------------------------


void AM::Network::m_do_read_udp() {

    memset(m_udprecv_data, 0, UDP_PROTO::MAX_PACKET_SIZE);
    m_udp_socket.async_receive_from(
            asio::buffer(m_udprecv_data, UDP_PROTO::MAX_PACKET_SIZE), m_udp_sender_endpoint,
            [this](std::error_code ec, std::size_t size) {
                if(ec) {
                    printf("[read_udp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }

                printf("\033[33m (UDP) <-\033[0m \"%s\" (%li)\n", m_udprecv_data, size);
                m_do_write_udp();    
            });
}

void AM::Network::m_do_write_udp() {
   
    if(m_udp_write_buffer.empty()) { return; }

    m_udp_socket.async_send_to(
            asio::buffer(m_udp_write_buffer.front(), m_udp_write_buffer.front().size()), m_udp_sender_endpoint,
            [this](std::error_code ec, std::size_t) {
                if(ec) {
                    printf("[write_udp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }

                if(!m_udp_write_buffer.empty()) {
                    m_udp_write_buffer.pop_front();
                }
                m_do_read_udp();
            });
}


