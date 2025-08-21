#ifndef AMBIENT3D_SERVER_HPP
#define AMBIENT3D_SERVER_HPP


#include <thread>
#include <mutex>
#include <atomic>

#include <asio.hpp>

#include "networking_agreements.hpp"

#include "tcp_session.hpp"
#include "udp_session.hpp"


using namespace asio::ip;


namespace AM {
    class Server {
        public:

            Server(asio::io_context& context, uint16_t port)
                : m_tcp_acceptor(context, tcp::endpoint(tcp::v4(), port)) {
                m_port = port;
            }

            ~Server();

            void start(asio::io_context& io_context);


        private:
            uint16_t m_port { 0 };
            std::atomic<bool> m_threads_exit { false };

            void         m_userinput_handler_th__func();
            std::thread  m_userinput_handler_th;

            // TCP is used for chat.
            tcp::acceptor m_tcp_acceptor;

            void _do_accept_TCP();

            // TODO:
            // UDP protocol is used for gameplay packets.

    };
};





#endif
