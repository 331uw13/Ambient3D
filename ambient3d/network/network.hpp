#ifndef AMBIENT3D_NETWORKING_HPP
#define AMBIENT3D_NETWORKING_HPP

#include <cstdint>
#include <thread>
#include <functional>

#include <deque>
#include <asio.hpp>

#include "networking_agreements.hpp"
#include "packet_ids.hpp"
#include "../gui/chatbox.hpp"


namespace AM {

    struct NetConnectCFG {
        const char* host;
        const char* tcp_port;
        const char* udp_port;

        // Called when data is received.
        std::function<void(
                uint8_t, // Red
                uint8_t, // Green
                uint8_t, // Blue
                const std::string&)> msg_recv_callback { NULL };
    };

    enum NetProto {
        TCP,
        UDP
    };

    class Network {

        public:

            Network(asio::io_context& io_context, const NetConnectCFG& cfg);
            ~Network() {}


            // The packet must be "prepared" before writing the packet data.
            // after all data needed is written the packet can be sent.
            void prepare_packet(AM::PacketID packet_id);
            void write_string  (const std::string& str);
            void write_float   (std::initializer_list<float> data);
            void write_int     (std::initializer_list<int> data);
            void send_packet   (AM::NetProto proto);

            //void write_string(AM::NetProto proto, AM::PacketID packet_id, const std::string& message);

            bool is_connected() { return m_connected; }
            void close(asio::io_context& io_context);
            

        private:
            bool m_connected { false };
            std::function<void(
                    uint8_t, // Red
                    uint8_t, // Green
                    uint8_t, // Blue
                    const std::string&)> m_msg_recv_callback;

            enum PacketStatus : int {
                NOT_PREPARED = 0,
                PREPARED,
                WRITTEN
            };

            PacketStatus            m_packet_status;
            
            char    m_packet_data[AM::MAX_PACKET_SIZE];
            size_t  m_packet_size;

            bool m_udp_data_ready_to_send;
            bool m_tcp_data_ready_to_send;

            bool m_write_packet_bytes(void* data, size_t data_sizeb);
            bool m_write_data_separator();
   

            std::thread m_event_handler_th;

            asio::ip::tcp::socket m_tcp_socket;
            void m_do_write_tcp();
            void m_do_read_tcp();


            asio::ip::udp::socket m_udp_socket;
            asio::ip::udp::endpoint m_udp_sender_endpoint;
            void m_do_read_udp();
            void m_do_write_udp();

            char m_udprecv_data[AM::MAX_PACKET_SIZE];
            char m_tcprecv_data[AM::MAX_PACKET_SIZE];
            
            void m_handle_tcp_packet(size_t sizeb);
            void m_handle_udp_packet(size_t sizeb);
    };
};


#endif
