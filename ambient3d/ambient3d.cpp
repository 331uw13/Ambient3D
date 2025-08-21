#include "ambient3d.hpp"
#include "internal_shaders.hpp"
#include "util.hpp"


AM::State::State(uint16_t win_width, uint16_t win_height, const char* title) {

    // Initialize raylib.
    
    InitWindow(win_width, win_height, title);
    SetExitKey(KEY_NULL);

    SetTargetFPS(1000);
    DisableCursor();

    GLSL_preproc_add_meminclude("GLSL_VERSION", "#version 430\n");
    GLSL_preproc_add_meminclude("AMBIENT3D_LIGHTS", I_Shaders::LIGHTS_GLSL);

    SetTraceLogLevel(LOG_ALL);

    // DEFAULT
    this->add_shader(LoadShaderFromMemory(
                GLSL_preproc(I_Shaders::DEFAULT_VERTEX).c_str(),
                GLSL_preproc(I_Shaders::DEFAULT_FRAGMENT).c_str()
                ));

    // DEFAULT_INSTANCED
    this->add_shader(LoadShaderFromMemory(
                GLSL_preproc(I_Shaders::DEFAULT_VERTEX, PREPROC_FLAGS::DEFINE__RENDER_INSTANCED).c_str(),
                GLSL_preproc(I_Shaders::DEFAULT_FRAGMENT).c_str()
                ));
    AM::init_instanced_shader(&this->shaders.back());
    

    // POST-PROCESSING
    this->add_shader(LoadShaderFromMemory(
                GLSL_preproc(I_Shaders::DEFAULT_VERTEX).c_str(),
                GLSL_preproc(I_Shaders::POSTPROCESS_FRAGMENT).c_str()
                ));
 
    // BLOOM_TRESHOLD
    this->add_shader(LoadShaderFromMemory(
                GLSL_preproc(I_Shaders::DEFAULT_VERTEX).c_str(),
                GLSL_preproc(I_Shaders::BLOOM_TRESH_FRAGMENT).c_str()
                ));  

    // BLOOM_DOWNSAMPLE_FRAGMENT
    this->add_shader(LoadShaderFromMemory(
                GLSL_preproc(I_Shaders::DEFAULT_VERTEX).c_str(),
                GLSL_preproc(I_Shaders::BLOOM_DOWNSAMPLE_FRAGMENT).c_str()
                ));  

    // BLOOM_UPSAMPLE_FRAGMENT
    this->add_shader(LoadShaderFromMemory(
                GLSL_preproc(I_Shaders::DEFAULT_VERTEX).c_str(),
                GLSL_preproc(I_Shaders::BLOOM_UPSAMPLE_FRAGMENT).c_str()
                ));  


    SetTraceLogLevel(LOG_NONE);

    // Create rendering targets.

    m_render_targets[RenderTargetIDX::RESULT]
        = LoadRenderTexture(win_width, win_height);

    m_render_targets[RenderTargetIDX::BLOOM_TRESHOLD]
        = LoadRenderTexture(win_width, win_height);
    
    m_render_targets[RenderTargetIDX::BLOOM_PRE_RESULT]
        = LoadRenderTexture(win_width, win_height);
    
    m_render_targets[RenderTargetIDX::BLOOM_RESULT]
        = LoadRenderTexture(win_width, win_height);


    
    int sample_res_X = win_width;
    int sample_res_Y = win_height;
    constexpr float scale_factor = 0.85f;


    for(size_t i = 0; i < m_bloom_samples.size(); i++) {
        
        m_bloom_samples[i] = LoadRenderTexture(sample_res_X, sample_res_Y);
        SetTextureFilter(m_bloom_samples[i].texture, TEXTURE_FILTER_BILINEAR);
        sample_res_X = round((float)sample_res_X * scale_factor);
        sample_res_Y = round((float)sample_res_Y * scale_factor);

    }
    


    SetTraceLogLevel(LOG_ALL);

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


    SetTraceLogLevel(LOG_NONE);
    // Unload render targets.
    for(size_t i = 0; i < m_render_targets.size(); i++) {
        UnloadRenderTexture(m_render_targets[i]);
    }
    for(size_t i = 0; i < m_bloom_samples.size(); i++) {
        UnloadRenderTexture(m_bloom_samples[i]);
    }
    SetTraceLogLevel(LOG_ALL);

    CloseWindow();
}


void AM::State::add_shader(const Shader& shader) {
    if(!IsShaderValid(shader)) {
        fprintf(stderr, "ERROR! Cant add broken shader to state.\n");
        return;
    }
    this->shaders.push_back(shader);
}

AM::Light** AM::State::add_light(const Light& light) {
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

void AM::State::set_vision_effect(float amount) {
    AMutil::clamp<float>(amount, 0.0f, 1.0f);
    AM::set_uniform_float(this->shaders[AM::ShaderIDX::POST_PROCESSING].id, "u_vision_effect", amount);
    AM::set_uniform_float(this->shaders[AM::ShaderIDX::POST_PROCESSING].id, "u_time", GetTime());
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

            
void AM::State::frame_begin() {
    BeginTextureMode(m_render_targets[RenderTargetIDX::RESULT]);
    ClearBackground(BLACK);
    BeginMode3D(this->player.cam);

    // TODO: Move these.
    //       User may need better control.
    if(this->mouse_captured) {
        this->player.update_camera();
    }
    this->player.update_movement(this);
    this->update_lights();
    this->terrain.find_new_chunks(
            this->player.chunk_x,
            this->player.chunk_z, 16);

}


void AM::State::m_render_bloom() {

    // Get bloom treshold texture.
    AMutil::resample_texture(
            this,
            /* TO */   m_bloom_samples[0],
            /* FROM */ m_render_targets[RenderTargetIDX::RESULT],
            -1, -1,
            -1, -1, 
            ShaderIDX::BLOOM_TRESHOLD
            );

    size_t p = 0;

    // Scale down and filter the texture each step.
    for(size_t i = 1; i < m_bloom_samples.size(); i++) {
        p = i - 1; // Previous sample.

        AMutil::resample_texture(
                this,
                /* TO */   m_bloom_samples[i],
                /* FROM */ m_bloom_samples[p],
                -1, -1,
                -1, -1, 
                ShaderIDX::BLOOM_DOWNSAMPLE_FILTER
                );
    }

    // Scale up and filter the texture each step.
    for(size_t i = m_bloom_samples.size()-2; i > 0; i--) {
        p = i + 1; // Previous sample.

        AMutil::resample_texture(
                this,
                /* TO */   m_bloom_samples[i],
                /* FROM */ m_bloom_samples[p],
                -1, -1,
                -1, -1, 
                ShaderIDX::BLOOM_UPSAMPLE_FILTER
                );
    }


    // Now the result is ready.

    AMutil::resample_texture(
            this,
            /* TO */   m_render_targets[RenderTargetIDX::BLOOM_RESULT],
            /* FROM */ m_bloom_samples[1],
            -1, -1,
            -1, -1, 
            -1
            );
}

/*
static void _draw_tex(const Texture2D& tex, int X, int Y, float scale, bool invert) {
    DrawTexturePro(tex,
            (Rectangle){
                0, 0, (float)tex.width, ((invert) ? -1.0 : 1.0) * (float)(tex.height)
            },
            (Rectangle){
                0, 0, (float)tex.width * scale, ((invert) ? -1.0 : 1.0) * (float)(tex.height * scale)
            },
            (Vector2){ -X, Y }, 0, WHITE);
}
*/
void AM::State::frame_end() {
    EndMode3D();
    this->draw_info();
    EndTextureMode();


    m_render_bloom();


    // Postprocessing.

    BeginDrawing();
    
    ClearBackground(BLACK);
    const Shader& shader = this->shaders[ShaderIDX::POST_PROCESSING];
    BeginShaderMode(shader);
    
    AM::set_uniform_sampler(shader.id, "texture_bloom", m_render_targets[RenderTargetIDX::BLOOM_RESULT].texture, 3);
    //AM::set_uniform_sampler(shader.id, "texture_bloom", m_bloom_samples[1].texture, 3);

    const int width  = m_render_targets[RenderTargetIDX::RESULT].texture.width;
    const int height = m_render_targets[RenderTargetIDX::RESULT].texture.height;
    DrawTextureRec(m_render_targets[RenderTargetIDX::RESULT].texture,
            (Rectangle){
                0, 0, (float)width, (float)-height
            },
            (Vector2){ 0, 0 }, WHITE);

    EndShaderMode();


    //_draw_tex(m_render_targets[RenderTargetIDX::RESULT].texture, GetScreenWidth()*0.5, 0, 0.5, true);
    //_draw_tex(m_render_targets[RenderTargetIDX::BLOOM_TRESHOLD].texture, 0, 0, 0.5, true);
    //_draw_tex(m_render_targets[RenderTargetIDX::BLOOM_RESULT].texture, 0, -50, 1.0, true);
    
    //_draw_tex(m_bloom_samples[2].texture, 0, -50, 0.9, true);
    
    //_draw_tex(m_bloom_samples[m_bloom_samples.size()-1].texture, 0, -300, 3.0, true);
   

    EndDrawing();
}



