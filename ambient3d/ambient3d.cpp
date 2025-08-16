#include "ambient3d.hpp"
#include "internal_shaders.hpp"
#include "util.hpp"


AM::State::State(uint16_t win_width, uint16_t win_height, const char* title) {

    // Initialize raylib.
    
    InitWindow(win_width, win_height, title);
    SetExitKey(KEY_NULL);

    SetTargetFPS(1000);
    SetTraceLogLevel(LOG_ALL);
    DisableCursor();

    
    // Default shader.
    this->add_shader(LoadShaderFromMemory(
                AMutil::combine_constchar({
                    I_Shaders::GLSL_VERSION,
                    I_Shaders::DEFAULT_VERTEX,
                }).c_str(),
                AMutil::combine_constchar({
                    I_Shaders::GLSL_VERSION,
                    I_Shaders::LIGHTS_GLSL,
                    I_Shaders::DEFAULT_FRAGMENT
                }).c_str()));

    m_lights_ubo.create(1, { 
            UBO_ELEMENT {
                .num = 64, .elem_sizeb = 48
            },
            UBO_ELEMENT {
                .num = 1, .elem_sizeb = 4
            }});
}

AM::State::~State() {

    for(Shader& shader : this->shaders) {
        UnloadShader(shader);
    }

    m_lights_ubo.free();
    
    CloseWindow();
}

    
void AM::State::add_shader(const Shader& shader) {
    if(!IsShaderValid(shader)) {
        fprintf(stderr, "ERROR! Cant add broken shader to state.\n");
        return;
    }
    this->shaders.push_back(shader);
}
/*
Light* AM::State::add_light(const Light& light) {
    m_lights.push_back(light);
    //Light* light_ptr = &m_lights.back();
    return light_ptr;
}
*/

AM::Light** AM::State::add_light(const Light& light) {
    //m_lights.push_back(light);
    //size_t new_size = m_lights.size();
    
    //m_lights_ubo.update_element(m_lights_ubo.size()-1, (void*)&new_size, sizeof(int));
    /*
    Light* light_ptr = &m_lights.back();
    m_light_ptrs.push_back(light_ptr);

    Light** result = &m_light_ptrs.back();
    */


    if(m_num_lights+1 >= AM::MAX_LIGHTS) {
        fprintf(stderr, "Increase the light array size or remove unused lights.\n");
        return NULL;
    }


    m_lights[m_num_lights] = light;
    m_light_ptrs[m_num_lights] = &m_lights[m_num_lights];
    Light** result = &m_light_ptrs[m_num_lights];

    m_num_lights++;
    // Update lights uniform buffer "num_lights"
    m_lights_ubo.update_element(m_lights_ubo.size()-1, (void*)&m_num_lights, sizeof(int));

    return result;
}

void AM::State::remove_light(Light** light) {
    if(!light) { return; }
    if(!(*light)) { return; }

    size_t id = (*light)->id;

    for(uint64_t i = id; i < m_num_lights-1; i++) {
        m_lights[i] = m_lights[i+1];
        if(id > 0) {
            m_lights[i].id--;
        }
        m_lights[i].force_update = true;
    }
    m_num_lights--;

    // Update pointers.
    for(size_t i = 0; i < m_num_lights; i++) {
        m_light_ptrs[i] = &m_lights[i];
    }

    *light = NULL;
    // Update lights uniform buffer "num_lights"
    m_lights_ubo.update_element(m_lights_ubo.size()-1, (void*)&m_num_lights, sizeof(int));
}


// Updates lights only if they have changed even little bit.
void AM::State::update_lights() {
    for(size_t i = 0; i < m_lights.size(); i++) {
        Light& light = m_lights[i];

        light.id = i;
        bool need_update = false;
        if(light.force_update) {
            m_lights_pframe_map[light.id] = light;
            light.force_update = false;
            need_update = true;
        }
        else {
            const auto search = m_lights_pframe_map.find(light.id);
            if(search == m_lights_pframe_map.end()) {
                need_update = true;
                m_lights_pframe_map.insert(std::make_pair(light.id, m_lights[i]));
            }
            else {
                if((need_update = !light.equal(search->second))) {
                    m_lights_pframe_map[light.id] = light;       
                }
            }
        }

        if(need_update) {
            float light_data[] = {
                light.pos.x,
                light.pos.y,
                light.pos.z,
                0.0f,

                (float)light.color.r / 255.0f,
                (float)light.color.g / 255.0f,
                (float)light.color.b / 255.0f,
                1.0f,

                light.strength,
                light.cutoff,
                0.0f,
                0.0f
            };

            m_lights_ubo.update_element(light.id, light_data, sizeof(light_data));
        }
    }
}

void AM::State::draw_text(int font_size, const char* text, int x, int y, const Color& color) {
    DrawTextEx(this->font, text, Vector2(x, y), font_size, 1.0f, color);
}

void AM::State::draw_info() {
    int text_y = 10;
    int text_x = 15;
    constexpr int y_add = 16;
    constexpr int font_size = 18;
    draw_text(font_size, TextFormat("FPS %i", GetFPS()), text_x, text_y, WHITE);
    text_y += y_add;
    draw_text(font_size, TextFormat("XYZ = (%0.2f, %0.2f, %0.2f)", 
                this->player.pos.x,
                this->player.pos.y,
                this->player.pos.z
                ), text_x, text_y, WHITE);
    text_y += y_add;
    draw_text(font_size, TextFormat("Chunk = (%i, %i)", 
                this->player.chunk_x,
                this->player.chunk_z), text_x, text_y, WHITE);
    text_y += y_add;
    draw_text(font_size, TextFormat("NoClip = %s", this->player.noclip ? "Yes" : "No"),
            text_x, text_y, WHITE);
    text_y += y_add;
    draw_text(font_size, TextFormat("OnGround = %s", this->player.on_ground ? "Yes" : "No"),
            text_x, text_y, WHITE);

}



