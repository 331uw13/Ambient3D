#ifndef AMBIENT3D_CHUNK_POS_HPP
#define AMBIENT3D_CHUNK_POS_HPP


namespace AM {

    struct ChunkPos {
        int x;
        int z;

        ChunkPos(){}
        ChunkPos(int _x, int _z) : x(_x), z(_z) {}
    };

};






#endif
