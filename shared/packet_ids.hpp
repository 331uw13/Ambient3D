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

        CHAT_MESSAGE, // (tcp only)

        // Server is going to send client their UUID
        // when they are connected.
        CLIENT_UUID,  // (tcp only)
       
        // This packet contains the player's requested position in the world.
        // It also has camera pitch and yaw information.
        // NOTE: The client side position is always a "request" to the server
        // - Format:
        // Byte offset  |  Value name
        // 0            :  PacketID
        // 4            :  Player X
        // 8            :  Player Y
        // 12           :  Player Z
        // 16           :  Camera Yaw
        // 20           :  Camera Pitch
        PLAYER_MOVEMENT_AND_CAMERA, // (udp only)



    };
};




#endif
