#ifndef AMBIENT3D_SERVER_HPP
#define AMBIENT3D_SERVER_HPP


#include <thread>
#include <mutex>
#include <atomic>

#include <asio.hpp>

#include "networking_agreements.hpp"

#include "tcp_session.hpp"
#include "udp_handler.hpp"


using namespace asio::ip;

typedef std::shared_ptr<AM::TCP_session> Client;


namespace AM {

    class Server {
        public:

            Server(asio::io_context& context, uint16_t tcp_port, uint16_t udp_port) :
                m_tcp_acceptor(context, tcp::endpoint(tcp::v4(), tcp_port)),
                m_udp_handler(context, udp_port) {}

            ~Server();

            void start(asio::io_context& io_context);

            std::mutex          tcp_clients_mtx;
            std::vector<Client> tcp_clients; 

            void remove_client(const Client& client);

        private:

            std::atomic<bool> m_threads_exit { false };

            void         m_userinput_handler_th__func();
            std::thread  m_userinput_handler_th;

            // TCP is used for chat.
            tcp::acceptor m_tcp_acceptor;
            void          m_do_accept_TCP();


            // UDP is used for gameplay packets.
            UDP_handler m_udp_handler;
    };
};





#endif
