#include <cstdio>
#include "renderable.hpp"
#include "ambient3d.hpp"

#include <rlgl.h>

#include "external/glad.h"


void AM::Renderable::load(const char* path, std::initializer_list<Shader> shaders) {
    if(!FileExists(path)) {
        fprintf(stderr, "ERROR! %s: \"%s\" doesnt exist or no permission to read.\n",
                __func__, path);
        return;
    }

    m_model = LoadModel(path);

    if(!IsModelValid(m_model)) {
        fprintf(stderr, "ERROR! %s: Failed to load \"%s\"\n", __func__, path);
        return;
    }

    size_t shader_idx = 0;
    for(int i = 0; i < m_model.meshCount; i++) {
        m_model.materials[0].shader = *(shaders.begin()+shader_idx);
        m_model.materials[m_model.meshMaterial[i]].shader = *(shaders.begin()+shader_idx);
        if(shader_idx+1 < shaders.size()) {
            shader_idx++;
        }
    }

    mesh_attribute(0, MeshAttrib{}); // Add default mesh attribute.
    this->transform = &m_model.transform;
    m_loaded = true;
}

void AM::Renderable::unload() {
    if(!m_loaded) { return; }
    UnloadModel(m_model);
    m_loaded = false;
}
            
void AM::Renderable::mesh_attribute(size_t mesh_index, const MeshAttrib& mesh_attrib) {
    if(m_mesh_attribs.size() <= mesh_index) {
        m_mesh_attribs.push_back(mesh_attrib);
        if(m_mesh_attribs.size() < mesh_index+1) {
            fprintf(stderr, "WARNING! %s: Renderable mesh attribute array size is only (%li)"
                    " but 'mesh_index' is (%li).\n", __func__, m_mesh_attribs.size(), mesh_index);
        }
    }
    else {
        m_mesh_attribs[mesh_index] = mesh_attrib;
    }
}


// TODO: Merge instanced rendering into one function:
// thing.render();
// or
// thing.render(AM::RENDER_INSTANCED, transforms);

void AM::Renderable::render() {
    if(!m_loaded) { return; }

    for(int i = 0; i < m_model.meshCount; i++) {
        Material& mat = m_model.materials[m_model.meshMaterial[i]];

        const MeshAttrib& mesh_attr = ((size_t)i < m_mesh_attribs.size())
            ? m_mesh_attribs[i] : MeshAttrib{};

        if(mesh_attr.render_backface) {
            rlDisableBackfaceCulling();
        }
        
        mat.maps[MATERIAL_MAP_DIFFUSE].color = mesh_attr.tint;

        AM::set_uniform_int(mat.shader.id, "u_affected_by_wind", mesh_attr.affected_by_wind);
        AM::set_uniform_float(mat.shader.id, "u_material_shine", mesh_attr.shine);
        DrawMesh(m_model.meshes[i], mat, m_model.transform);
        
        if(mesh_attr.render_backface) {
            rlEnableBackfaceCulling();
        }
        if(mesh_attr.affected_by_wind) {
            AM::set_uniform_int(mat.shader.id, "u_affected_by_wind", 0);
        }
    }
}
            

void AM::Renderable::render_instanced(Matrix* transforms, size_t size) {
    if(!m_loaded) { return; }

    for(int i = 0; i < m_model.meshCount; i++) {
        Material& mat = m_model.materials[m_model.meshMaterial[i]];

        const MeshAttrib& mesh_attr = ((size_t)i < m_mesh_attribs.size())
            ? m_mesh_attribs[i] : MeshAttrib{};

        if(mesh_attr.render_backface) {
            rlDisableBackfaceCulling();
        }

        AM::set_uniform_int(mat.shader.id, "u_affected_by_wind", mesh_attr.affected_by_wind);
        DrawMeshInstanced(m_model.meshes[i], mat, transforms, size);
        
        if(mesh_attr.render_backface) {
            rlEnableBackfaceCulling();
        }
        if(mesh_attr.affected_by_wind) {
            AM::set_uniform_int(mat.shader.id, "u_affected_by_wind", 0);
        }
    }  
}



