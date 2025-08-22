#ifndef AMBIENT3D_NETWORKING_HPP
#define AMBIENT3D_NETWORKING_HPP

#include <cstdint>
#include <thread>

#include <deque>
#include <asio.hpp>

#include "networking_agreements.hpp"
#include "../gui/chatbox.hpp"


namespace AM {

    struct NetConnectCFG {
        const char* host;
        const char* tcp_port;
        const char* udp_port_client;
        const char* udp_port_server;

        AM::Chatbox* tcp_chatbox;
    };

    enum NetProto {
        TCP,
        UDP
    };

    class Network {

        public:

            Network(asio::io_context& io_context, const NetConnectCFG& cfg);
            ~Network() {}


            void write_string(AM::NetProto proto, const std::string& message);

            bool is_connected() { return m_connected; }
            void close(asio::io_context& io_context);
            

        private:
            AM::Chatbox* m_tcp_chatbox { NULL };
            bool m_connected { false };


            std::deque<std::string> m_tcp_write_buffer;
            std::deque<std::string> m_udp_write_buffer;

            std::thread m_event_handler_th;

            asio::ip::tcp::socket m_tcp_socket;
            void m_do_write_tcp();
            void m_do_read_tcp();


            asio::ip::udp::socket m_udp_socket;
            asio::ip::udp::endpoint m_udp_sender_endpoint;
            void m_do_read_udp();
            void m_do_write_udp();

            char m_udprecv_data[UDP_PROTO::MAX_PACKET_SIZE];
            char m_tcprecv_data[TCP_PROTO::MAX_PACKET_SIZE];
    };

};






#endif
