#include <cstdio>

#include "animation.hpp"



void AM::Animation::load(const char* path) {
    if(m_loaded) {
        fprintf(stderr, "ERROR! %s: Animation for \"%s\" seems to be already loaded.\n",
                __func__, path);
        return;
    }

    int anim_count = 0;
    m_anim_data = NULL;
    m_anim_data = LoadModelAnimations(path, &anim_count);

    m_loaded = ((anim_count > 0) && this->data);
    m_num_anims = (uint32_t)anim_count;

    if(!m_loaded) {
        return;
    }
    
    m_animation_speeds = new float[m_num_anims];
    for(uint32_t i = 0; i < m_num_anims; i++) {
        m_animation_speeds[i] = 0.05f; // Default value. It is probably changed by user.
    }
}

void AM::Animation::set_animation_speed(uint32_t anim_index, float anim_speed) {
    
}

void AM::Animation::update_animation(uint32_t anim_index, float frame_time) {
}


