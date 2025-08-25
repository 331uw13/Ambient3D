
#include <cstdio>
#include <iostream>
#include <chrono>

#include "server.hpp"


        
AM::Server::~Server() {

    printf("Server closed.\n");
}


void AM::Server::start(asio::io_context& io_context) {

    m_udp_handler.start();

    m_do_accept_TCP();

    // Start the event handler.
    std::thread event_handler([](asio::io_context& context) {
        context.run();
    }, std::ref(io_context));


    // Start user input handler.
    m_userinput_handler_th = 
        std::thread(&AM::Server::m_userinput_handler_th__func, this);


    // Input handler may tell to shutdown.
    m_userinput_handler_th.join();

    io_context.stop();
    event_handler.join();
}

            
void AM::Server::remove_client(const Client& client) {
    this->tcp_clients_mtx.lock();
    bool found = false;
    size_t i = 0;
    for(; i < this->tcp_clients.size(); i++) {
        if(client->uuid.equals(this->tcp_clients[i]->uuid)) {
            found = true;
            break;
        }
    }

    if(found) {
        this->tcp_clients.erase(this->tcp_clients.begin() + i);
    }

    this->tcp_clients_mtx.unlock();
}

void AM::Server::m_do_accept_TCP() {
    
    // Context will call this lambda when connection arrives.
    m_tcp_acceptor.async_accept(
            [this](std::error_code ec, tcp::socket socket) {
                printf("ACCEPT\n");

                if(!ec) {
                    this->tcp_clients_mtx.lock();
                    
                    this->tcp_clients.push_back(std::make_shared<TCP_session>(std::move(socket), this));
                    const Client client = this->tcp_clients.back();
                    client->start();

                    char uuid_buf[AM::UUID_LENGTH * 4] = { 0 };
                    for(size_t i = 0; i < AM::UUID_LENGTH; i++) {
                        snprintf(&uuid_buf[i], 3, "%x", client->uuid[i]);
                    }


                    printf("UUID: %s\n", uuid_buf);
                    this->tcp_clients_mtx.unlock();
                }

                m_do_accept_TCP();
            });
}
            
void AM::Server::broadcast_tcp(const std::string& str) {
    this->tcp_clients_mtx.lock();
    
    for(size_t i = 0; i < this->tcp_clients.size(); i++) {
        this->tcp_clients[i]->write(AM::PacketID::CHAT_MESSAGE, str);
    }

    this->tcp_clients_mtx.unlock();
}



void AM::Server::m_userinput_handler_th__func() {
    std::string input;
    input.reserve(256);

    while(!m_threads_exit) {

        std::cin >> input;
        if(input.empty()) { continue; }

        if(input == "end") {
            m_threads_exit = true;
        }
        else
        if(input == "clear") {
            printf("\033[2J\033[H");
            fflush(stdout);
        }
        else
        if(input == "online") {
            this->tcp_clients_mtx.lock();
            printf("Online players: %li\n", this->tcp_clients.size());
            this->tcp_clients_mtx.unlock();
        }
        else {
            printf(" Unknown command.\n");
        }

    }

    // NOTE: When the above while loop is exited.
    //       The server will shutdown.
}


