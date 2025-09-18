#ifndef AMBIENT3D_NETWORKING_HPP
#define AMBIENT3D_NETWORKING_HPP

#include <cstdint>
#include <thread>
#include <functional>
#include <map>
#include <nlohmann/json.hpp>

#include <deque>
#include <asio.hpp>

#include "packet_writer.hpp"
#include "network_player.hpp"
#include "server_config.hpp"
#include "../item_manager.hpp"
#include "../terrain/terrain.hpp"

#include "../gui/chatbox.hpp"

using json = nlohmann::json;


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

    class State;

    class Network {

        public:

            Network(asio::io_context& io_context, const NetConnectCFG& cfg);
            ~Network() {}

            

            // The packet can be written with functions from
            // './shared/packet_writer.*'
            // This is going to be saved here so it dont need to be
            // always allocated again on the stack.
            Packet packet;
            
            void   send_packet(AM::NetProto proto);

            std::map<int/* player_id*/, N_Player>  players;

            bool is_connected() { return m_connected; }
            bool is_fully_connected() { return m_fully_connected; }
            void close(asio::io_context& io_context);

            AM::ServerCFG server_cfg;

            int player_id;

            void set_engine_state(AM::State* state) {
                m_engine = state;
            }
            /*
            void set_item_manager(AM::ItemManager* item_manager) {
                m_engine_item_manager = item_manager;
            }

            void set_terrain(AM::Terrain* terrain) {
                m_engine_terrain = terrain;
            }
            */

        private:
            bool m_connected { false };
            bool m_fully_connected { false };

            std::function<void(
                    uint8_t, // Red
                    uint8_t, // Green
                    uint8_t, // Blue
                    const std::string&)> m_msg_recv_callback;

            AM::State* m_engine;
                
            bool m_udp_data_ready_to_send;
            bool m_tcp_data_ready_to_send;
           
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
