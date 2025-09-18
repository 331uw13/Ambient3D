#include <cstdio>

#include "chunk.hpp"
#include "perlin_noise.hpp"


static float noise_func(float x, float z) {
    float level  = perlin_noise_2D(x, z) * 3.0;

    return level;
}


void AM::Chunk::generate(const AM::ServerCFG& server_cfg, const AM::ChunkPos& chunk_pos, int seed) {
    this->pos = chunk_pos;

    const size_t num_points = (server_cfg.chunk_size+1) * (server_cfg.chunk_size+1);
    this->height_points = new float[num_points];

    size_t point_index = 0;
    for(int local_z = 0; local_z <= server_cfg.chunk_size; local_z++) {
        for(int local_x = 0; local_x <= server_cfg.chunk_size; local_x++) {
        
            float height_level = noise_func(
                    (float)(local_x + chunk_pos.x * server_cfg.chunk_size) / 10.0f,
                    (float)(local_z + chunk_pos.z * server_cfg.chunk_size) / 10.0f);

            this->height_points[point_index] = height_level;
            point_index++;
        }
    }

    m_loaded = true;
}
            
bool AM::Chunk::unload() {
    if(!m_loaded) {
        fprintf(stderr, "ERROR! Trying to unload not loaded chunk.\n");
        return false;
    }

    delete[] height_points;
    height_points = NULL;
    m_loaded = false;
    return true;
}


