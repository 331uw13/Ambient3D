#include <cstdio>
#include <cmath>

#include "terrain.hpp"
#include "../server.hpp"


void AM::Terrain::add_chunk(const AM::Chunk& chunk) {
    auto inserted = this->chunk_map.insert(std::make_pair(chunk.get_id(), chunk)).first;
    if(inserted == this->chunk_map.end()) {
        fprintf(stderr, "ERROR! %s: Failed to add chunk (id = %i)\n",
                __func__, chunk.get_id());
        this->chunk_map_mutex.unlock();
        return;
    }
}

void AM::Terrain::remove_chunk(const AM::Chunk& chunk) {
    auto chunk_it = this->chunk_map.find(chunk.get_id());
    if(chunk_it != this->chunk_map.end()) {
        chunk_it->second.unload();
        this->chunk_map.erase(chunk_it);
    }
}

            
void AM::Terrain::delete_terrain() {
    size_t num_unloaded = 0;
    for(auto it = this->chunk_map.begin(); it != this->chunk_map.end(); ++it) {
        it->second.unload();
        num_unloaded++;
    }

    printf("[WORLD_GEN]: Unloaded %li chunks\n", num_unloaded);
}
            
AM::ChunkPos AM::Terrain::get_chunk_pos(float world_x, float world_z) {
    return AM::ChunkPos(
            (int)floor(world_x / m_server->config.chunk_size) % m_server->config.chunk_size,
            (int)floor(world_z / m_server->config.chunk_size) % m_server->config.chunk_size
            );
}

AM::ChunkID AM::Terrain::get_chunk_id(const ChunkPos& chunk_pos) {
    return chunk_pos.z * m_server->config.chunk_size + chunk_pos.x;    
}


void AM::Terrain::foreach_chunk_nearby(float world_x, float world_z, 
        std::function<void(const AM::Chunk*, const AM::ChunkPos&, AM::ChunkID)> callback) {

    const int chunk_size_half = m_server->config.chunk_size / 2;

    for(int chunk_lZ = -chunk_size_half; chunk_lZ < chunk_size_half; chunk_lZ++) {
        for(int chunk_lX = -chunk_size_half; chunk_lX < chunk_size_half; chunk_lX++) { 

            AM::ChunkPos chunk_pos
                = this->get_chunk_pos(world_x, world_z);
            
            chunk_pos.x += chunk_lX;
            chunk_pos.z += chunk_lZ;

            AM::ChunkID chunk_id 
                = this->get_chunk_id(chunk_pos);

            auto chunk_it = this->chunk_map.find(chunk_id);
            if(chunk_it != this->chunk_map.end()) {
                callback(&chunk_it->second, chunk_pos, chunk_id);
            }
            else {
                callback(NULL, chunk_pos, chunk_id);
            }
        }
    }
}


