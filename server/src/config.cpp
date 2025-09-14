#include <nlohmann/json.hpp>
#include <fstream>

#include "config.hpp"

using json = nlohmann::json;



AM::ServerCFG::ServerCFG(const char* json_cfg_path) {

    std::fstream stream(json_cfg_path);
    if(!stream.is_open()) {
        fprintf(stderr, "ERROR! %s: Failed to open server configuration file (%s)\n",
                __func__, json_cfg_path);
        return;
    }

    json data = json::parse(stream);


    this->tcp_port = data["tcp_port"].template get<int>();
    this->udp_port = data["udp_port"].template get<int>();
    this->item_list_path = data["item_list_path"].template get<std::string>();
    this->item_near_distance = data["item_near_distance"].template get<float>();


}



