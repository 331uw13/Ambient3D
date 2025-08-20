#ifndef AMBIENT3D_RENDERABLE_OBJECT_HPP
#define AMBIENT3D_RENDERABLE_OBJECT_HPP


#include <raylib.h>
#include <vector>
#include <cstdint>


namespace AM {

    struct MeshAttrib {
        // NOTE: This only works with shader programs
        //       which implement wind in their vertex shaders.
        bool affected_by_wind   { false };
        
        bool render_backface    { false };
        Color tint              { WHITE };
        float shine             { 3.0f };
    };

    class Renderable {
        public:

            void load(const char* path, std::initializer_list<Shader> shaders);
            void unload();
            void render();
            void render_instanced(Matrix* transforms, size_t size);
            void mesh_attribute(size_t mesh_index, const MeshAttrib& mesh_attrib);
            bool is_loaded() { return m_loaded; }
            Matrix* transform;

        private:
            std::vector<MeshAttrib> m_mesh_attribs;
            bool m_loaded;
            Model m_model;
    };
}



#endif
