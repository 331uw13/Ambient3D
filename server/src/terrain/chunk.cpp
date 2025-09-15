#include <cstdio>

#include "chunk.hpp"
#include "perlin_noise.hpp"


static float noise_func(float x, float z) {
    float level 
        = perlin_noise_2D(x, z) * 3.0
        + perlin_noise_2D(x*0.1, z*0.1)*20.0
        + ((perlin_noise_2D(x*0.2, z*0.2)*120.0)
        * perlin_noise_2D(x*0.05, z*0.05))
        + ((perlin_noise_2D(x*0.05, z*0.05)*500.0)
        * perlin_noise_2D(x*0.01, z*0.01));
 
    return level;
}


void AM::Chunk::generate(const AM::ServerCFG& server_cfg, int seed, int x, int z) {
    m_id = z * server_cfg.chunk_size + x;
    m_pos = ChunkPos(x, z);

    const size_t num_points = (server_cfg.chunk_size+1) * (server_cfg.chunk_size+1);
    this->height_points = new float[num_points];

    size_t point_index = 0;
    for(int local_z = 0; local_z <= server_cfg.chunk_size; local_z++) {
        for(int local_x = 0; local_x <= server_cfg.chunk_size; local_x++) {
        
            float height_level = noise_func((float)(x+local_x), (float)(z+local_z));

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


