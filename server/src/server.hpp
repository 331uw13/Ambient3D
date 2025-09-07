#ifndef AMBIENT3D_SERVER_HPP
#define AMBIENT3D_SERVER_HPP


#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <asio.hpp>

#include "networking_agreements.hpp"

//#include "tcp_session.hpp"
#include "udp_handler.hpp"
#include "player.hpp"


using namespace asio::ip;

// TODO: REMOVE THIS!!!!!!  (read below text)
//typedef std::shared_ptr<AM::TCP_session> Client;


// to be honest this could be structured ALOT better,
// but just testing and it can be refactored later...

// TCP_session   (each client)
// PlayerData    (each client)
//
// We want to get players by their ID
//
// Could create Player class
// which would hold the PlayerData and TCP_session.
// They could be saved in std::map<player_id, std::shared_ptr<Player>>
// 
// get_tcp_session_by_id AND get_player_by_id
// can be replaced with:
// get_player_by_id
// ^- which would return Player*


namespace AM {

    class Server {
        public:

            Server(asio::io_context& context, uint16_t tcp_port, uint16_t udp_port) :
                m_tcp_acceptor(context, tcp::endpoint(tcp::v4(), tcp_port)),
                m_udp_handler(context, udp_port) {}

            ~Server();

            void start(asio::io_context& io_context);
            
            std::mutex  players_mutex; // <- tcp_clients_mtx
            std::map<int/*player_id*/, Player> players; // <- tcp_clients; 


            void        remove_player     (int player_id);
            AM::Player* get_player_by_id  (int player_id);

            void broadcast_msg(AM::PacketID packet_id, const std::string& str);


            //Client      get_tcp_session_by_id(int player_id);
            //PlayerData* get_player_by_id(int player_id);

            std::atomic<bool> show_debug_info { false };

        private:

            std::atomic<bool> m_keep_threads_alive { true };

            //std::map<int/*player_id*/, PlayerData> m_player_data_map;

            void         m_userinput_handler_th__func();
            std::thread  m_userinput_handler_th;

            void         m_update_loop_th__func();
            std::thread  m_update_loop_th;

            // TCP is used for chat.
            tcp::acceptor m_tcp_acceptor;
            void          m_do_accept_TCP();

            int           m_next_player_id { 1 };

            // UDP is used for gameplay packets.
            UDP_handler m_udp_handler;
    };
};





#endif
