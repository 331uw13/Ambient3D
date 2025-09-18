#ifndef AMBIENT3D_SERVER_TERRAIN_HPP
#define AMBIENT3D_SERVER_TERRAIN_HPP

#include <mutex>
#include <unordered_map>
#include <functional>

#include "chunk.hpp"


namespace AM {

    class Server;

    class Terrain {
        public:
            std::mutex                                  chunk_map_mutex;
            std::unordered_map<AM::ChunkPos, AM::Chunk> chunk_map;

            void add_chunk(const AM::Chunk& chunk);
            void remove_chunk(const AM::Chunk& chunk);
            void delete_terrain();

            AM::ChunkPos  get_chunk_pos(float world_x, float world_z);

            

            void foreach_chunk_nearby(float world_x, float world_z, 
                    std::function<void(const AM::Chunk*, const AM::ChunkPos&)> callback);
            
            void set_server(AM::Server* server) { m_server = server; }

        private:

            AM::Server* m_server;

    };


}

#endif
