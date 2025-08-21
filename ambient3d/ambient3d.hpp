#ifndef AMBIENT3D_HPP
#define AMBIENT3D_HPP

#include <raylib.h>
#include <cstdint>
#include <map>
#include <array>
#include <functional>

#include "player.hpp"
#include "terrain.hpp"
#include "shader_util.hpp"
#include "uniform_buffer.hpp"
#include "light.hpp"
#include "renderable.hpp"
#include "glsl_preproc.hpp"
#include "util.hpp"


// TODO: Make networking optional to use ?

#include "network/network.hpp"



namespace AM {

    static constexpr int NUM_BLOOM_SAMPLES = 16;
    static constexpr int MAX_LIGHTS = 64;

    enum ShaderIDX : int {
        DEFAULT,
        DEFAULT_INSTANCED,
        POST_PROCESSING,
        BLOOM_TRESHOLD,
        BLOOM_DOWNSAMPLE_FILTER,
        BLOOM_UPSAMPLE_FILTER,
        // ...
    };


    class State {
        public:
            State(uint16_t win_width, uint16_t win_height, const char* title);
            ~State();

            Font     font;
            Player   player;
            Terrain  terrain;

            void    frame_begin();
            void    frame_end();

            // Shaders in this array will be unloaded when the state is destructed.
            std::vector<Shader> shaders;
            void add_shader(const Shader& shader);

            Light** add_light(const Light& light);
            void    remove_light(Light** light);
            void    update_lights();
            void    draw_info();
            void    draw_text(int font_size, const char* text, int x, int y, const Color& color);

            // When amount is close to 0.0 small distortions happen
            // but when it reaches 0.5 "blinking" starts happening
            // and it gets stronger towards 1.0
            void    set_vision_effect(float amount);

            bool mouse_captured { true };


        private:

            bool m_connected_to_server;

            enum RenderTargetIDX : int {
                RESULT,
                BLOOM_TRESHOLD,
                BLOOM_PRE_RESULT,
                BLOOM_RESULT,

                NUM_TARGETS
            };


            void m_render_bloom();

            std::array<RenderTexture2D, RenderTargetIDX::NUM_TARGETS>
                m_render_targets;

            std::array<RenderTexture2D, NUM_BLOOM_SAMPLES>
                m_bloom_samples;



            UniformBuffer m_lights_ubo;
            std::array<Light, MAX_LIGHTS> m_lights;
            std::array<Light*, MAX_LIGHTS> m_light_ptrs { NULL };
            size_t m_num_lights { 0 };

            std::map<int/*light ID*/, Light> m_lights_pframe_map; // Previous frame lights.
            
    };

};




#endif
