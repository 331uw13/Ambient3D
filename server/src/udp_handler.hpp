#ifndef AMBIENT3D_UDP_SESSION_HPP
#define AMBIENT3D_UDP_SESSION_HPP


#include <asio.hpp>
using namespace asio::ip;


#include "networking_agreements.hpp"


namespace AM {

    class UDP_handler {
        public:

            UDP_handler(asio::io_context& io_context, uint16_t port)
                : m_socket(io_context, udp::endpoint(udp::v4(), port)) {}

            void start() { m_do_read(); }

        private:

            void m_handle_received_packet(size_t sizeb);
            void m_do_read();
            void m_do_write();

            udp::endpoint m_sender_endpoint;
            udp::socket m_socket;

            char m_data[AM::MAX_PACKET_SIZE];

    };

};



#endif
