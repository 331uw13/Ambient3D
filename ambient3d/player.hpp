#ifndef AMBIENT3D_PLAYER_HPP
#define AMBIENT3D_PLAYER_HPP

#include <raylib.h>
#include <rcamera.h>
#include <cstdint>

namespace AM {
    class State;
    class Player {
        public:
            Player();
            ~Player();
            
            //==== Public Variables ====
            Camera   cam;
            Vector3  pos;
            Vector3  vel; // Velocity.
            Vector3  pos_prevframe;
            bool     noclip;
            float    noclip_speed;
            bool     on_ground;
            float    gravity;
            float    speed;
            float    height;
            float    cam_sensetivity;
            float    cam_yaw;
            float    cam_pitch;
            Vector3  movement;
            Vector3  forward;
            int      chunk_x;
            int      chunk_z;
            int8_t   num_jumps_in_air;
            Vector3  terrain_normal;

            //==== Public Functions ====
            void update_movement(State* st);
            void update_camera();

            void jump();

        private:

            //==== Private Functions ====
            void m_update_gravity(State* st);
            void m_update_slide();
  
            //==== Private Variables ====
            float m_slide_boost;
            bool m_jumped;
            int  m_num_jumps_left;
            bool m_sliding;
            
            float m_Yvelocity_in_air;
    };
};




#endif
