#ifndef AMBIENT3D_TCP_SESSION_HPP
#define AMBIENT3D_TCP_SESSION_HPP


#include <memory>
#include <vector>
#include <string>

#include <asio.hpp>
using namespace asio::ip;


// From '../../shared'
#include "networking_agreements.hpp"
#include "packet_ids.hpp"
#include "uuid.hpp"


namespace AM {
    class Server;

    class TCP_session : public std::enable_shared_from_this<TCP_session> {
        public:
            TCP_session(tcp::socket socket, AM::Server* server);

            UUID uuid;
            
            void start() { m_do_read(); }
            void write(AM::PacketID packet_id, const std::string& msg);


        private:

            std::vector<std::string> m_write_buffer;

            //void m_do_write(const std::string& msg);
            void m_do_read();

            tcp::socket  m_socket;
            Server*      m_server;

            void m_handle_received_packet(size_t sizeb);
            char m_data[AM::MAX_PACKET_SIZE]; // <- Received data. (TODO: Should probably rename.)
            
            // Used for sending data.
            char   m_packet_data[AM::MAX_PACKET_SIZE];
            size_t m_packet_size { 0 };
    };
};




#endif
