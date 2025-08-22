#ifndef AMBIENT3D_TCP_SESSION_HPP
#define AMBIENT3D_TCP_SESSION_HPP


#include <memory>
#include <vector>
#include <string>

#include <asio.hpp>
using namespace asio::ip;


// From '../../shared'
#include "networking_agreements.hpp"
#include "uuid.hpp"


namespace AM {
    class Server;

    class TCP_session : public std::enable_shared_from_this<TCP_session> {
        public:
            TCP_session(tcp::socket socket, AM::Server* server);

            UUID uuid;
            
            void start() { m_do_read(); }
            void write(const std::string& msg);


        private:

            std::vector<std::string> m_write_buffer;

            //void m_do_write(const std::string& msg);
            void m_do_read();

            tcp::socket  m_socket;
            Server*      m_server;

            char m_data[TCP_PROTO::MAX_PACKET_SIZE];
    };
};




#endif
