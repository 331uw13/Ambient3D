#ifndef AMBIENT3D_TCP_SESSION_HPP
#define AMBIENT3D_TCP_SESSION_HPP


#include <memory>

#include <asio.hpp>
using namespace asio::ip;


#include "networking_agreements.hpp"


namespace AM {

    class TCP_session : public std::enable_shared_from_this<TCP_session> {
        public:

            TCP_session(tcp::socket socket)
                : m_socket(std::move(socket)) {}

            void start() { m_do_read(); }


        private:

            void m_do_read();
            void m_do_write();

            tcp::socket m_socket;

            char m_data[TCP_PROTO::MAX_PACKET_SIZE];
    };
};




#endif
