#ifndef AMBIENT3D_PACKET_IDS_HPP
#define AMBIENT3D_PACKET_IDS_HPP



// Client and server will send a packet
// with this kind of format:
//
// <TCP/UDP packet id> (4 bytes reserved.)
// Packet id is followed by collection of data,
//
// The data may sometimes be separated by 'PACKET_DATA_SEPARATOR'
// If data size may not be fixed. 
// But this depends alot on what kind of data is being sent/received
//

namespace AM {


    enum PacketID : int {
        NONE = 0,

        CHAT_MESSAGE,    // (tcp only)
        SERVER_MESSAGE,  // (tcp only)

        // Server will send player their id when they are connected (via TCP)
        // Then the client will reply with their ID via UDP protocol,
        // that process saves the client's udp endpoint.
        PLAYER_ID,

        // ^-- After that if successful the server will respond with
        // this packet via TCP
        PLAYER_ID_HAS_BEEN_SAVED,


        // This packet contains the player's requested position in the world.
        // It also has camera pitch, yaw and animation information.
        // NOTE: The client side position is always a "request" to the server
        // - Format:
        // Byte offset  |  Value name
        // 0            :  Packet ID        (int)
        // 4            :  Player ID        (int)
        // 8            :  Player X         (float)
        // 12           :  Player Y         (float)
        // 16           :  Player Z         (float)
        // 20           :  Camera Yaw       (float)
        // 24           :  Camera Pitch     (float)
        // 28           :  Animation ID     (int)
        PLAYER_MOVEMENT_AND_CAMERA, // (udp only)

       
    };

    namespace PacketSize {
        static constexpr size_t PLAYER_MOVEMENT_AND_CAMERA = 28;
        static constexpr size_t PLAYER_ID = 4;
    };
};




#endif
