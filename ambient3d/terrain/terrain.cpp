#include <cstring>
#include <lz4.h>
#include <cstdio>

#include "terrain.hpp"



void AM::Terrain::allocate_regenbuf(size_t num_bytes) {
    if(!m_chunkdata_regenbuf) {
        m_chunkdata_regenbuf = new char[num_bytes];
        m_chunkdata_regenbuf_memsize = num_bytes;
        printf("[TERRAIN]: Allocated chunkdata regen buffer (%li bytes)\n", num_bytes);
    }
}

void AM::Terrain::free_regenbuf() {
    if(m_chunkdata_regenbuf) {
        delete[] m_chunkdata_regenbuf;
        m_chunkdata_regenbuf = NULL;
    }
}

void AM::Terrain::update_chunkdata_queue() {
    if(!m_chunkdata_regenbuf) {
        return;
    }
    
    
    m_chunkdata_queue_mutex.lock();
        
    // TODO: request resend if something fails

    for(size_t i = 0; i < m_chunkdata_queue.size(); i++) {
        mChunkData* chunkdata = &m_chunkdata_queue[i];
        if(chunkdata->num_bytes >= m_chunkdata_regenbuf_memsize) {
            fprintf(stderr, "ERROR! %s: Received chunk data has unexpectedly too many bytes (%li)."
                    " Has allocated %li bytes\n",
                    __func__, chunkdata->num_bytes, m_chunkdata_regenbuf_memsize);
            continue;
        }

        // chunk_id + chunk_x + chunk_z = 4*3 bytes.
        if(chunkdata->num_bytes <= (sizeof(int)*3)) {
            fprintf(stderr, "ERROR! %s: Received chunk data is too small to be valid.\n",
                    __func__);
            continue;
        }


        // One while loop iteration will uncompress one chunk's data and loads mesh.
        size_t byte_offset = 0;
        while(byte_offset < m_chunkdata_regenbuf_memsize) {

            
            if(m_chunkdata_regenbuf_size > 0) {
                memset(m_chunkdata_regenbuf, 0, m_chunkdata_regenbuf_size);
                m_chunkdata_regenbuf_size = 0;
            }

            ssize_t decompressed_size
                = LZ4_decompress_safe(
                        chunkdata->bytes.data(), // Source.
                        m_chunkdata_regenbuf,    // Destination.
                        chunkdata->num_bytes,    // Source size.
                        m_chunkdata_regenbuf_memsize); // Destination max size.


            break;
        }


    }

    m_chunkdata_queue.clear();
    m_chunkdata_queue_mutex.unlock();
}

void AM::Terrain::add_chunkdata_to_queue(char* compressed_data, size_t sizeb) {
    m_chunkdata_queue_mutex.lock();
    
    if(!compressed_data || (sizeb == 0)) {
        return;
    }
    
    mChunkData& data_added = m_chunkdata_queue.emplace_back();

    memmove(data_added.bytes.data(), compressed_data, sizeb);
    data_added.num_bytes = sizeb;
    m_chunkdata_queue_mutex.unlock();
}

