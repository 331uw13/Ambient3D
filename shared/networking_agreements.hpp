#ifndef AMBIENT3D_NETWORKING_AGREEMENTS_HPP
#define AMBIENT3D_NETWORKING_AGREEMENTS_HPP

#include <cstdint>



namespace TCP_PROTO {
    static constexpr uint32_t MAX_PACKET_SIZE = 1024;
    
};


namespace UDP_PROTO {
    static constexpr uint32_t MAX_PACKET_SIZE = 512;
};


#endif
