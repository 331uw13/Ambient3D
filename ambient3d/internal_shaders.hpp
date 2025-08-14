#ifndef AMBIENT3D_INTERNAL_SHADERS_HPP
#define AMBIENT3D_INTERNAL_SHADERS_HPP




namespace AM {
    namespace I_Shaders {

        /*
        static constexpr const char*
            TEMPLATE = R"(
            #version 430

            void main() {
                
            }

            )";
        */

        static constexpr const char*
            GLSL_VERSION = "#version 430\n";

        static constexpr const char*
            DEFAULT_VERTEX = R"(
            in vec3 vertex_pos;
            in vec2 vertex_texcoord;
            in vec3 vertex_normal;
            in vec4 vertex_color;

            uniform mat4 mvp;
            uniform mat4 matModel;
            uniform mat4 matNormal;

            out vec2 frag_texcoord;
            out vec4 frag_color;
            out vec3 frag_normal;
            out vec3 frag_position;

            void main() {
                frag_texcoord = vertex_texcoord;
                frag_color = vertex_color;
                frag_position = vec3(matModel*vec4(vertex_pos, 1.0));
                frag_normal = normalize(vec3(matNormal * vec4(vertex_normal, 1.0)));

                gl_Position = mvp * vec4(vertex_pos, 1.0);
            }

            )";

        static constexpr const char*
            DEFAULT_FRAGMENT = R"(
            in vec2 frag_texcoord;
            in vec4 frag_color;
            in vec3 frag_normal;
            in vec3 frag_position;

            uniform sampler2D texture0;
            uniform vec4 colDiffuse;

            uniform vec3 u_view_pos;

            out vec4 out_color;

            void main() {
                vec3 lights = compute_lights(frag_position, frag_normal, u_view_pos);
                out_color = vec4(0.5, 0.5, 0.5, 1.0) * vec4(lights, 1.0);
                out_color = vec4(pow(out_color.rgb, vec3(1.0/1.6)), out_color.a);
            }
            )";

        static constexpr const char*
            LIGHTS_GLSL = R"(
            #define MAX_LIGHTS 64

            struct Light {
                vec4 pos;
                vec4 color;
                vec4 settings;
            };

            layout(std140, binding = 1) uniform lights_ubo {
                Light lights[MAX_LIGHTS];
                int num_lights;
            };

            // Returns: RGB.
            vec3 compute_lights(
                vec3 frag_pos,
                vec3 frag_n,
                vec3 view_pos
            ){
                vec3 final = vec3(0, 0, 0);
                vec3 normal = normalize(frag_n);
                //normal = mix(normal, vec3(0.0, 1.0, 0.0), 0.3);
                for(int i = 0; i < num_lights; i++) {
                    vec3 light_pos = lights[i].pos.xyz;
                    vec3 light_dir = normalize(light_pos - frag_pos);
                    vec3 view_dir = normalize(view_pos - frag_pos);
                    vec3 halfway_dir = normalize(light_dir - view_dir);

                    vec3 light_color = lights[i].color.rgb;
                    float radius = lights[i].settings.x;
                    float cutoff = lights[i].settings.y;


                    // Diffuse.
                    float diff = max(dot(normal, light_dir), 0.0);
                    vec3 diffuse = diff * light_color;

                    // Specular.
                    float spec = pow(max(dot(view_dir, reflect(-light_dir, normal)), 0.0), 6.0);
                    vec3 specular = spec * mix(vec3(1.0, 1.0, 1.0), light_color, 0.5);

                    // Attenuation.
                    float L = 1.0;
                    float Q = 2.3;
                    float dist = distance(frag_pos, light_pos) / radius;
                    dist = pow(dist, cutoff);
                    float a = 1.0 / (2.0 + L * dist + Q * (dist * dist));
                    
                    diffuse *= a;
                    specular *= (a*0.5);
                    vec3 ambient = a * ((light_color * dist) * 0.15);

                    final += diffuse + specular + ambient;
                }

                return clamp(final, vec3(0), vec3(1));
            }

            )";
    };
};






#endif
