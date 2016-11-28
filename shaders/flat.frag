#version 410

in vec2 var_texcoords;

layout(location=0) out vec4 out_Color;
uniform sampler2D texture_obj;


void main () {
    out_Color = texture(texture_obj, var_texcoords);
}

