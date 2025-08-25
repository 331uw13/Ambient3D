#include <cstdio>
#include <string>
#include <iostream>

#include "network.hpp"
#include "packet_parser.hpp"



void AM::Network::m_handle_tcp_packet(size_t sizeb) {
    
    AM::PacketID packet_id = AM::parse_network_packet(m_tcprecv_data, sizeb);

    switch(packet_id) {
        case AM::PacketID::CHAT_MESSAGE:
            printf("[CHAT]: %s\n", m_tcprecv_data);
            m_msg_recv_callback(255, 255, 255, m_tcprecv_data);
            break;


        // ...
    }

}


void AM::Network::m_handle_udp_packet(size_t sizeb) {
}





AM::Network::Network(asio::io_context& io_context, const NetConnectCFG& cfg)
    : m_tcp_socket(io_context),
      m_udp_socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0))
{
    if(!cfg.msg_recv_callback) {
        fprintf(stderr, "WARNING! %s: No message received callback.\n",__func__);
    }

    try {
        m_msg_recv_callback = cfg.msg_recv_callback;

        asio::ip::tcp::resolver tcp_resolver(io_context);
        asio::connect(m_tcp_socket, tcp_resolver.resolve(cfg.host, cfg.tcp_port));

        asio::ip::udp::resolver udp_resolver(io_context);
        m_udp_sender_endpoint = *udp_resolver.resolve(cfg.host, cfg.udp_port).begin();
    
        memset(m_tcprecv_data, 0, AM::MAX_PACKET_SIZE);
        memset(m_udprecv_data, 0, AM::MAX_PACKET_SIZE);

        m_connected = true;
        m_do_read_tcp();
        m_do_write_udp();

        // Start event handler.
        m_event_handler_th = std::thread([](asio::io_context& context){
                context.run();
                }, std::ref(io_context));

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


bool AM::Network::m_write_packet_bytes(void* data, size_t data_sizeb) {
    if((m_packet_size + data_sizeb) >= AM::MAX_PACKET_SIZE) {
        m_packet_status = PacketStatus::NOT_PREPARED;
        fprintf(stderr, "ERROR! \"%s\" @ %s: The packet is too large to write anymore info. "
                "It has been marked as 'NOT_PREPARED'!\n", __FILE__, __func__);
        return false;
    }

    memmove(m_packet_data + m_packet_size,
            data,
            data_sizeb);

    m_packet_size += data_sizeb; 

    return true;
}
   
bool AM::Network::m_write_data_separator() {
    if((m_packet_size + 1) >= AM::MAX_PACKET_SIZE) {
        m_packet_status = PacketStatus::NOT_PREPARED;
        fprintf(stderr, "ERROR! \"%s\" @ %s: The packet is too large to write anymore info. "
                "It has been marked as 'NOT_PREPARED'!\n", __FILE__, __func__);
        return false;
    }

    m_packet_data[m_packet_size++] = AM::PACKET_DATA_SEPARATOR;

    return true;
}


void AM::Network::prepare_packet(AM::PacketID packet_id) {
    memset(m_packet_data, 0, 
            (m_packet_size < AM::MAX_PACKET_SIZE) ? m_packet_size : AM::MAX_PACKET_SIZE);
    
    m_packet_size = 0;
    if(!m_write_packet_bytes((void*)&packet_id, sizeof(AM::PacketID))) {
        return;
    }

    m_packet_status = PacketStatus::PREPARED;
}


void AM::Network::write_string(const std::string& str) {
    if(m_packet_status == PacketStatus::NOT_PREPARED) { return; }
    
    for(size_t i = 0; i < str.size(); i++) {
        if(i >= AM::MAX_PACKET_SIZE) {
            fprintf(stderr, "ERROR! \"%s\" @ %s: Too much data for packet, Some of it is ignored.\n",
                    __FILE__, __func__);
            break;
        }

        m_packet_data[i+4] = str[i];
        m_packet_size++;
    }
    m_packet_status = PacketStatus::WRITTEN;
}


void AM::Network::write_float(std::initializer_list<float> data) {
    if(m_packet_status == PacketStatus::NOT_PREPARED) { return; }
    for(auto it = data.begin(); it != data.end(); ++it) {
        if(!m_write_packet_bytes((void*)it, sizeof(float))) {
            return;
        }
    }
    m_packet_status = PacketStatus::WRITTEN;
}

void AM::Network::write_int(std::initializer_list<int> data) {
    if(m_packet_status == PacketStatus::NOT_PREPARED) { return; }
    for(auto it = data.begin(); it != data.end(); ++it) {
        if(!m_write_packet_bytes((void*)it, sizeof(float))) {
            return;
        }
    }
    m_packet_status = PacketStatus::WRITTEN;
}

void AM::Network::send_packet(AM::NetProto proto) {
    if(m_packet_status <= PacketStatus::NOT_PREPARED) {
        fprintf(stderr, "ERROR! \"%s\" @ %s: The packet is not been prepared.\n", __FILE__, __func__);
        return;
    }
    if(m_packet_status <= PacketStatus::PREPARED) {
        fprintf(stderr, "ERROR! \"%s\" @ %s: The packet doesnt contain any data.\n", __FILE__, __func__);
        return;
    }


    if(proto == AM::NetProto::TCP) {
        m_tcp_data_ready_to_send = true;
        m_do_write_tcp();
    }
    else 
    if(proto == AM::NetProto::UDP) {
        m_udp_data_ready_to_send = true;
        m_do_write_udp();
    }

    m_packet_status = PacketStatus::NOT_PREPARED;
}


// TCP ------------------------------------------------------

void AM::Network::m_do_read_tcp() {
    
    memset(m_tcprecv_data, 0, AM::MAX_PACKET_SIZE);

    m_tcp_socket.async_read_some(asio::buffer(m_tcprecv_data, AM::MAX_PACKET_SIZE),
            [this](std::error_code ec, std::size_t size) {
                if(ec) {
                    printf("[read_tcp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }

                m_handle_tcp_packet(size);
                /*
                if(m_msg_recv_callback) {
                    m_msg_recv_callback(255, 255, 255, m_tcprecv_data);
                }
 
                printf("\033[33m (TCP) <-\033[0m \"%s\" (%li)\n", m_tcprecv_data, size);
                */
                m_do_read_tcp();
            });

}


void AM::Network::m_do_write_tcp() {
    if(!m_tcp_data_ready_to_send) {
        return;
    }
    m_tcp_data_ready_to_send = false;

    asio::async_write(m_tcp_socket, 
            asio::buffer(m_packet_data, m_packet_size),
            [this](std::error_code ec, std::size_t /*size*/) {
                if(ec) {
                    printf("[write_tcp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }               
            });
}




// UDP ------------------------------------------------------


void AM::Network::m_do_read_udp() {

    memset(m_udprecv_data, 0, AM::MAX_PACKET_SIZE);
    m_udp_socket.async_receive_from(
            asio::buffer(m_udprecv_data, AM::MAX_PACKET_SIZE), m_udp_sender_endpoint,
            [this](std::error_code ec, std::size_t size) {
                if(ec) {
                    printf("[read_udp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }

                m_handle_udp_packet(size);
                m_do_read_udp(); 
            });
}

void AM::Network::m_do_write_udp() {
    if(!m_udp_data_ready_to_send) {
        return;
    }
    m_udp_data_ready_to_send = false;

    m_udp_socket.async_send_to(
            asio::buffer(m_packet_data, m_packet_size), m_udp_sender_endpoint,
            [this](std::error_code ec, std::size_t) {
                if(ec) {
                    printf("[write_udp](%i): %s\n", ec.value(), ec.message().c_str());
                    return;
                }
            });
}


