#ifndef AMBIENT3D_SERVER_CHUNK_HPP
#define AMBIENT3D_SERVER_CHUNK_HPP

#include "server_config.hpp"


namespace AM {

    typedef int ChunkID;

    struct ChunkPos {
        int x;
        int z;

        ChunkPos(){}
        ChunkPos(int _x, int _z) : x(_x), z(_z) {}
    };

    // grid which contains ids for example 0 is nothing, 1 is tree, 2 is rock.. etc


    class Chunk {
        public:
            // One dimensional array which 
            // contains (chunk_size * chunk_size) number of height points.
            float* height_points { NULL };


            void     generate(const AM::ServerCFG& server_cfg, int seed, int x, int z);
            bool     unload();
            
            bool     is_loaded() const { return m_loaded; }
            ChunkID  get_id() const { return m_id; }
            ChunkPos get_pos() const { return m_pos; } 

        private:

            bool      m_loaded { false };
            ChunkID   m_id;
            ChunkPos  m_pos;

    };

};



#endif
