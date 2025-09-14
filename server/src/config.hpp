#ifndef AMBIENT3D_SERVER_CONFIG_DEF_HPP
#define AMBIENT3D_SERVER_CONFIG_DEF_HPP

#include <string>


namespace AM {

    struct ServerCFG {
        ServerCFG(){}
        ServerCFG(const char* json_cfg_path);

        int tcp_port;
        int udp_port;
        std::string item_list_path;
        float item_near_distance;
  
    };

};


#endif
