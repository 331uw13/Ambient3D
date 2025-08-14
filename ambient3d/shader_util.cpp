#include <raylib.h>
#include <rlgl.h>
#include <map>
#include <cstdio>

#include "shader_util.hpp"
#include "external/glad.h"


namespace {
    //         shader id,    uniform name,   uniform location
    static std::map<int, std::map<const char*, int>>
        g_shader_map;

    static constexpr int LOCATION_NOTFOUND = -1;
    static int _find_location(int shader_id, const char* u_name) {
        int location = LOCATION_NOTFOUND;
        const auto search1 = g_shader_map.find(shader_id);
        if(search1 == g_shader_map.end()) {
            location = rlGetLocationUniform(shader_id, u_name);
            g_shader_map[shader_id][u_name] = location;
            if(location >= 0) {
                printf("+ Found uniform location for '%s': %i\n", u_name, location);
            }
            else {
                fprintf(stderr, "ERROR! Could not find location for '%s' ShaderID=%i\n", 
                        u_name, shader_id);
            }
        }
        else {
            location = g_shader_map[shader_id][u_name];
        }

        return location;
    }
};


// TODO: Implement something when the value has not changed, it dont need to be updated.
 
void AM::set_uniform_int(int shader_id, const char* uniform_name, int value) {
    glUseProgram(shader_id);
    glUniform1i(_find_location(shader_id, uniform_name), value);
}   

void AM::set_uniform_float(int shader_id, const char* uniform_name, float value) {
    glUseProgram(shader_id);
    glUniform1f(_find_location(shader_id, uniform_name), value);
}

void AM::set_uniform_vec2(int shader_id, const char* uniform_name, const Vector2& value) {
    glUseProgram(shader_id);
    glUniform2f(_find_location(shader_id, uniform_name), value.x, value.y);
}

void AM::set_uniform_vec3(int shader_id, const char* uniform_name, const Vector3& value) {
    glUseProgram(shader_id);
    glUniform3f(_find_location(shader_id, uniform_name), value.x, value.y, value.z);
}

void AM::set_uniform_matrix(int shader_id, const char* uniform_name, const Matrix& value) {
    glUseProgram(shader_id);
    glUniformMatrix3fv(_find_location(shader_id, uniform_name), 1, false, &value.m0);
}

    
