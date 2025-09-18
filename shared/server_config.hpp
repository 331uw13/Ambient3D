#ifndef AMBIENT3D_SERVER_CONFIG_DEF_HPP
#define AMBIENT3D_SERVER_CONFIG_DEF_HPP

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;


namespace AM {

    struct ServerCFG {
        ServerCFG(){}
        ServerCFG(const char* json_cfg_path);
        void parse_from_memory(const json& data);

        int tcp_port;
        int udp_port;
        std::string item_list_path;
        float item_near_distance;
        int chunk_size;
        int render_distance;
        int chunkdata_uncompressed_max_bytes;
        std::string json_data;
    };

};


#endif
