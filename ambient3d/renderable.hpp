#ifndef AMBIENT3D_RENDERABLE_OBJECT_HPP
#define AMBIENT3D_RENDERABLE_OBJECT_HPP


#include <raylib.h>
#include <vector>
#include <cstdint>


namespace AM {

    struct MeshAttrib {

        // TODO: Move 'mesh_index' here.

        // NOTE: This only works with shader programs
        //       which implement wind in their vertex shaders.
        bool affected_by_wind   { false };
        
        bool render_backface    { false };
        Color tint              { WHITE };
        float shine             { 0.3f };
   
    };

    // Load options for Renderable.
    enum RenderableLoadOpt {
        LF_DEFAULT,

        // Enables ability to change individual mesh transform matrices
        // through 'Renderable::mesh_transform' array.
        LF_ENABLE_MESH_TRANSFORMS, 

    };

    static constexpr size_t RENDERABLE_MAX_NAME_SIZE = 24;
    class Renderable {
        public:

            void load(const char* path,
                    std::initializer_list<Shader>  shaders,
                    RenderableLoadOpt load_options = RenderableLoadOpt::LF_DEFAULT);

            // Guranteed to be NULL terminated.
            char name[RENDERABLE_MAX_NAME_SIZE];

            void unload();
            void render();
            void mesh_attribute(size_t mesh_index, const MeshAttrib& mesh_attrib);

            bool is_loaded() { return m_loaded; }
            Matrix* transform;

            // Size is number of meshes in model. 
            // Only available if ENABLE_MESH_TRANSFORMS is set used.
            Matrix* mesh_transforms { NULL };

            uint32_t num_meshes()  { return (uint32_t)m_model.meshCount; }
            Model* get_model()     { return &m_model; }
        private:

            MeshAttrib* m_mesh_attribs;
            //std::vector<MeshAttrib> m_mesh_attribs;
            bool m_loaded;
            Model m_model;

            void m_name_from_path(const char* path);
    };
}



#endif
