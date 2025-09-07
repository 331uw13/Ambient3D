
#include <cstdio>
#include <iostream>
#include <chrono>

#include "server.hpp"


        
AM::Server::~Server() {

    printf("Server closed.\n");
}


void AM::Server::start(asio::io_context& io_context) {

    m_udp_handler.start(this);
    m_do_accept_TCP();

    // Start the event handler.
    std::thread event_handler([](asio::io_context& context) {
        context.run();
    }, std::ref(io_context));


    // Start user input handler.
    m_userinput_handler_th = 
        std::thread(&AM::Server::m_userinput_handler_th__func, this);

    // Start the updater loop.
    m_update_loop_th =
        std::thread(&AM::Server::m_update_loop_th__func, this);

    // Input handler may tell to shutdown.
    m_userinput_handler_th.join();

    io_context.stop();
    event_handler.join();
    m_update_loop_th.join();
}

            
void AM::Server::remove_player(int player_id) {
    this->players_mutex.lock();

    const auto search = this->players.find(player_id);
    if(search != this->players.end()) {
        this->players.erase(search);
    }

    this->players_mutex.unlock();

    char msgbuf[512+1] = { 0 };
    snprintf(msgbuf, 512, "Player %i left the server", player_id);
    this->broadcast_msg(PacketID::SERVER_MESSAGE, msgbuf);
}

void AM::Server::m_do_accept_TCP() {
    
    // Context will call this lambda when connection arrives.
    m_tcp_acceptor.async_accept(
            [this](std::error_code ec, tcp::socket socket) {
                if(ec) {
                    printf("[accept](%i): %s\n", ec.value(), ec.message().c_str());
                
                    m_do_accept_TCP();
                    return;
                }

                this->players_mutex.lock();

                int player_id = m_next_player_id++; // <- TODO: MAKE SURE THIS WILL NEVER OVERFLOW!
                Player player(std::make_shared<TCP_session>(std::move(socket), this, player_id));
                player.id = player_id;

                player.tcp_session->start();
                this->players.insert(std::make_pair(player_id, player));
                this->players_mutex.unlock();

                printf("[+] Player joined! ID = %i\n", player_id);

                // Send player id.
                AM::packet_prepare  (&player.tcp_session->packet, AM::PacketID::PLAYER_ID);
                AM::packet_write_int(&player.tcp_session->packet, { player_id });
                player.tcp_session->send_packet();

                m_do_accept_TCP();
            });

}
            
AM::Player* AM::Server::get_player_by_id(int player_id) {
    this->players_mutex.lock();
    const auto search = this->players.find(player_id);
    if(search == this->players.end()) {
        fprintf(stderr, "ERROR! No player found with ID: %i\n", player_id);
        this->players_mutex.unlock();
        return NULL;
    }

    this->players_mutex.unlock();
    return (AM::Player*)&search->second;
}


void AM::Server::broadcast_msg(AM::PacketID packet_id, const std::string& msg) {
    this->players_mutex.lock();

    for(auto it = this->players.begin(); it != this->players.end(); ++it) {
        Player* p = &it->second;
        AM::packet_prepare(&p->tcp_session->packet, packet_id);
        AM::packet_write_string(&p->tcp_session->packet, msg);
        p->tcp_session->send_packet();
    }

    this->players_mutex.unlock();
}


void AM::Server::m_update_loop_th__func() {
    while(m_keep_threads_alive) {
        this->players_mutex.lock();

        // Tell players each others position, camera yaw and pitch.
        
        // TODO: Wallhacks, "ESP" cheats can be prevented
        // by only telling the player's position when they can see them
        // NOTE to self: think about ways how to bypass this.

        for(auto itA = this->players.begin();
                itA != this->players.end(); ++itA) {
            const Player* player = &itA->second;

            for(auto itB = this->players.begin();
                    itB != this->players.end(); ++itB) {
                const Player* p = &itB->second;
                if(p->id == player->id) { continue; }

                AM::packet_prepare(&m_udp_handler.packet, AM::PacketID::PLAYER_MOVEMENT_AND_CAMERA);
                AM::packet_write_int(&m_udp_handler.packet, { player->id });
                AM::packet_write_float(&m_udp_handler.packet, {
                            player->pos.x,
                            player->pos.y,
                            player->pos.z,
                            player->cam_yaw,
                            player->cam_pitch
                        });
                AM::packet_write_int(&m_udp_handler.packet, { player->anim_id });

                m_udp_handler.send_packet(p->id);
            }
        }

        this->players_mutex.unlock();

        std::this_thread::sleep_for(
                std::chrono::milliseconds(30));
    }
}


void AM::Server::m_userinput_handler_th__func() {
    std::string input;
    input.reserve(256);

    while(m_keep_threads_alive) {

        std::cin >> input;
        if(input.empty()) { continue; }

        // TODO: Create better system for commands.

        if(input == "end") {
            m_keep_threads_alive = false;
        }
        else
        if(input == "clear") {
            printf("\033[2J\033[H");
            fflush(stdout);
        }
        else
        if(input == "show_debug") {
            this->show_debug_info = true;
        }
        else
        if(input == "hide_debug") {
            this->show_debug_info = false;
        }
        else
        if(input == "online") {
            this->players_mutex.lock();
            printf("Online players: %li\n", this->players.size());
            this->players_mutex.unlock();
        }
        else {
            printf(" Unknown command.\n");
        }

    }

    // NOTE: When the above while loop is exited.
    //       The server will shutdown.
}
            

