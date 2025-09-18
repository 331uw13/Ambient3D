#ifndef AMBIENT3D_CHUNK_HPP
#define AMBIENT3D_CHUNK_HPP

#include <cstddef>

#include "raylib.h"
#include "chunk_pos.hpp"


namespace AM {

    typedef int ChunkID;


    class Chunk {
        public:
            AM::ChunkID id;
            AM::ChunkPos pos;
            float* height_points;

            void load_mesh(float* points, size_t points_sizeb);
            void unload();

            void render();

        private:

            Mesh m_mesh;

    };

};



#endif
