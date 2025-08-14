#ifndef AMBIENT3D_PLAYER_HPP
#define AMBIENT3D_PLAYER_HPP

#include <raylib.h>
#include <rcamera.h>


namespace AM {
    class Player {
        public:
            Player();
            ~Player();
            
            //==== Public Variables ====
            Camera   cam;
            Vector3  pos;
            Vector3  vel;
            float    speed;
            float    cam_sensetivity;
            float    cam_yaw;
            float    cam_pitch;
            Vector3  movement;
            int      chunk_x;
            int      chunk_z;

            //==== Public Functions ====
            void update_movement();
            void update_camera();


        private:

            //==== Private Functions ====
            // ...

            //==== Private Variables ====
            // ...
    
    };
};




#endif
