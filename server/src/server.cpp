
#include <cstdio>
#include <iostream>
#include <chrono>

#include "server.hpp"


        
AM::Server::~Server() {

    printf("Server closed.\n");
}


void AM::Server::start(asio::io_context& io_context) {

    printf("Listening on port %i ...\n", m_port);

    _do_accept_TCP();

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


void AM::Server::_do_accept_TCP() {
    
    // Context will call this lambda when connection arrives.
    m_tcp_acceptor.async_accept(
            [this](std::error_code ec, tcp::socket socket){
                printf("ACCEPT\n");

                if(!ec) {
                    std::make_shared<TCP_session>(std::move(socket))->start();
                }

                _do_accept_TCP();
            });
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
        else {
            printf(" Unknown command.\n");
        }

    }

    // NOTE: When the above while loop is exited.
    //       The server will shutdown.
}



