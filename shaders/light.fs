#version 410

in vec3 var_Normal;
in vec2 var_texcoords;
in vec3 var_Position;

layout(location=0) out vec4 out_Color;
uniform sampler2D texture_obj;
uniform vec3 camPos;
uniform vec4 lightPos;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

vec3 blinn_phong(vec3 kd) {
    vec4 pos_world = vec4(var_Position, 1);
    vec3 normal_world = normalize(var_Normal);
    pos_world /= pos_world.w;
    vec3 light_dir = vec3(0,0,0);
    if (lightPos.w == 0) {
      light_dir = normalize(lightPos.xyz);
    }
    else {
      light_dir = normalize(lightPos.xyz - pos_world.xyz);
    }
    vec3 cam_dir = camPos - pos_world.xyz;
    cam_dir = normalize(cam_dir);

    float ndotl = max(dot(normal_world, light_dir), 0.0);
    vec3 diffContrib = lightDiffuse * kd * ndotl;

    vec3 R = reflect( -light_dir, normal_world );
    float eyedotr = max(dot(cam_dir, R), 0.0);
    vec3 specContrib = pow(eyedotr, 3.0f) *
                       1.0f * lightDiffuse;

    return diffContrib + specContrib;
    //return  + vec4(diffContrib + specContrib, alpha);
}

void main () {
    vec4 tex_samp = texture(texture_obj, var_texcoords);
    out_Color = vec4(lightAmbient+blinn_phong(tex_samp.xyz), tex_samp.w);
}


