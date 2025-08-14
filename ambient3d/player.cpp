#include <cstdio>

#include "player.hpp"
#include "chunk.hpp"

#include <raymath.h>            



AM::Player::Player() {
    this->cam.position = Vector3(0.0f, 0.0f, 0.0f);
    this->cam.target = Vector3(0.1f, 0.0f, 0.0f);
    this->cam.up = Vector3(0.0f, 1.0f, 0.0f);
    this->cam.fovy = 70.0f;
    this->cam.projection = CAMERA_PERSPECTIVE;
    this->vel = Vector3(0.0f, 0.0f, 0.0f);
    this->pos = Vector3(0.0f, 0.0f, 1.0f);
    this->speed = 100.0f;
    this->cam_sensetivity = 0.0035f;
}

AM::Player::~Player() {
}


            

void AM::Player::update_movement() {
    const float dt = GetFrameTime();
    Vector3 cam_dir = Vector3(
                cos(this->cam_pitch) * sin(this->cam_yaw),
                sin(this->cam_pitch),
                cos(this->cam_pitch) * cos(this->cam_yaw)
                );
    Vector3 right = Vector3CrossProduct(cam_dir, Vector3(0.0f, 1.0f, 0.0f));

    float speed = this->speed;

    if(IsKeyDown(KEY_LEFT_ALT)) {
        speed *= 100.0;
    } 

    speed *= dt;

    if(IsKeyDown(KEY_W)) {
        this->movement.x += cam_dir.x * speed;
        this->movement.z += cam_dir.z * speed;
    }
    if(IsKeyDown(KEY_S)) {
        this->movement.x -= cam_dir.x * speed;
        this->movement.z -= cam_dir.z * speed;
    }
    if(IsKeyDown(KEY_D)) {
        this->movement += right * speed;
    }
    if(IsKeyDown(KEY_A)) {
        this->movement -= right * speed;
    }
    if(IsKeyDown(KEY_SPACE)) {
        this->movement.y += speed;
    }
    if(IsKeyDown(KEY_LEFT_SHIFT)) {
        this->movement.y -= speed;
    }

    this->pos += this->movement * dt;

    float F = pow(0.95, dt * 120);
    this->movement.x *= F;
    this->movement.y *= F;
    this->movement.z *= F;

    this->cam.position = this->pos;
    this->cam.target = Vector3Add(this->pos, 
            Vector3(
                cos(this->cam_pitch) * sin(this->cam_yaw),
                sin(this->cam_pitch),
                cos(this->cam_pitch) * cos(this->cam_yaw)
                ));

    this->chunk_x = (int)floor(this->pos.x / (AM::CHUNK_SIZE * AM::CHUNK_SCALE));
    this->chunk_z = (int)floor(this->pos.z / (AM::CHUNK_SIZE * AM::CHUNK_SCALE));
}

 
void AM::Player::update_camera() {
    Vector2 md = GetMouseDelta();
    this->cam_yaw += (-md.x * this->cam_sensetivity);
    this->cam_pitch += (-md.y * this->cam_sensetivity);
}

