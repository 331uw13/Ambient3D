#ifndef AMBIENT3D_SERVER_HPP
#define AMBIENT3D_SERVER_HPP


#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <array>
#include <asio.hpp>
#include <nlohmann/json.hpp>

#include "networking_agreements.hpp"
#include "config.hpp"

#include "udp_handler.hpp"
#include "player.hpp"

#include "item_base.hpp"


using json = nlohmann::json;
using namespace asio::ip;



namespace AM {

    static constexpr int TICK_SPEED_MS = 40;

    class Server {
        public:

            Server(asio::io_context& context, const AM::ServerCFG& cfg)/* :
                m_tcp_acceptor(context, tcp::endpoint(tcp::v4(), cfg.tcp_port)),
                m_udp_handler(context, cfg.udp_port) {}*/;
            
            /*
            Server(asio::io_context& context, uint16_t tcp_port, uint16_t udp_port) :
                m_tcp_acceptor(context, tcp::endpoint(tcp::v4(), tcp_port)),
                m_udp_handler(context, udp_port) {}
            */
            ~Server();

            void start(asio::io_context& io_context);
            
            std::mutex                          players_mutex;
            std::map<int/*player_id*/, Player>  players;
           
            // 'item_templates' is an array of item info loaded from items/item_list.json
            // when server spawns an item 
            // the corresponding item template is copied and
            // modified before its then inserted into dropped_items
            std::array<AM::ItemBase, AM::NUM_ITEMS>            item_templates;
            std::unordered_map<int/*item uuid*/, AM::ItemBase> dropped_items;
            std::mutex                                         dropped_items_mutex;

            void        remove_player     (int player_id);
            AM::Player* get_player_by_id  (int player_id);

            void spawn_item(AM::ItemID item_id, int count, const Vec3& pos);
            void broadcast_msg(AM::PacketID packet_id, const std::string& str);

            void load_item_template(const char* entry_name, AM::ItemID item_id);

            std::atomic<bool> show_debug_info { false };

        private:

            std::atomic<bool> m_keep_threads_alive { true };

            void         m_update_players();
            void         m_update_items();

            bool         m_parse_item_list(const std::string& item_list_path);
            json         m_item_list;

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
    
            ServerCFG m_config;
    };
};





#endif
