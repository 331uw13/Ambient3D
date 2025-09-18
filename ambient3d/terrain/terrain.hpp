#ifndef AMBIENT3D_TERRAIN_HPP
#define AMBIENT3D_TERRAIN_HPP

#include <unordered_map>
#include <mutex>
#include <array>
#include <vector>
#include <cstddef>

#include "chunk.hpp"
#include "networking_agreements.hpp"


namespace AM {

    class Terrain {
        public:

            std::unordered_map<AM::ChunkID, AM::Chunk> chunk_map;

            // 'allocate_regenbuf' is called from 
            // "./network/network.cpp" handle_tcp_packet(size_t). case AM::PacketID::SERVER_CONFIG
            // because the server and client will agree how many bytes
            // the max uncompressed size for chunk data is.
            void allocate_regenbuf(size_t num_bytes);
            void free_regenbuf();
            void add_chunkdata_to_queue(char* compressed_data, size_t sizeb);
            
            // IMPORTANT NOTE: must be called from main thread.
            void update_chunkdata_queue();

        private:

            struct mChunkData {
                mChunkData() {}
                std::array<char, AM::MAX_PACKET_SIZE> bytes;
                size_t                                num_bytes { 0 };
            };

            std::mutex              m_chunkdata_queue_mutex;
            std::vector<mChunkData> m_chunkdata_queue;

            // Used for decompressing chunk data.
            char* m_chunkdata_regenbuf { NULL };
            size_t m_chunkdata_regenbuf_memsize { 0 };
            size_t m_chunkdata_regenbuf_size { 0 };
    };
};



#endif
