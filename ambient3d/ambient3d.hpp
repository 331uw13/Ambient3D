#ifndef AMBIENT3D_HPP
#define AMBIENT3D_HPP

#include <raylib.h>
#include <cstdint>
#include <map>
#include <array>

#include "player.hpp"
#include "terrain.hpp"
#include "shader_util.hpp"
#include "uniform_buffer.hpp"
#include "light.hpp"




namespace AM {

    static constexpr int MAX_LIGHTS = 64;

    enum ShaderIDX : int {
        DEFAULT,
        // ...
    };

    class State {
        public:
            State(uint16_t win_width, uint16_t win_height, const char* title);
            ~State();

            Font     font;
            Player   player;
            Terrain  terrain;


            // Shaders in this array will be unloaded when the state is destructed.
            std::vector<Shader> shaders;
            void add_shader(const Shader& shader);

            Light** add_light(const Light& light);
            void    remove_light(Light** light);
            void    update_lights();



        private:

            UniformBuffer m_lights_ubo;
            std::array<Light, MAX_LIGHTS> m_lights;
            std::array<Light*, MAX_LIGHTS> m_light_ptrs;
            size_t m_num_lights;

            std::map<int/*light ID*/, Light> m_lights_pframe_map; // Previous frame lights.
    };

};




#endif
