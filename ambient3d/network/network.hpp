#ifndef AMBIENT3D_NETWORKING_HPP
#define AMBIENT3D_NETWORKING_HPP

#include <cstdint>
#include <thread>

#include <asio.hpp>

#include "networking_agreements.hpp"


namespace AM {

    struct NetConnectCFG {
        const char* host;
        const char* tcp_port;
      //const char* udp_port;
    };

    class Network {

        public:

            Network(asio::io_context& io_context, const NetConnectCFG& cfg)
                : m_tcp_socket(io_context)
            {
                printf("Connecting to %s:%s\n", cfg.host, cfg.tcp_port);
                try {
                    asio::ip::tcp::resolver resolver(io_context);
                    asio::connect(m_tcp_socket, resolver.resolve(cfg.host, cfg.tcp_port));
            
                    m_connected = true;
                    m_do_read_tcp();

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

            ~Network() {}

            void write_message(const std::string& message);

            bool is_connected() { return m_connected; }
            void close(asio::io_context& io_context);

        private:
            bool m_connected { false };


            std::vector<std::string> m_tcp_write_buffer;

            std::thread m_event_handler_th;
            asio::ip::tcp::socket m_tcp_socket;

            void m_do_read_tcp();
            void m_do_write_tcp();

            char m_tcprecv_data[TCP_PROTO::MAX_PACKET_SIZE];
    };

};






#endif
