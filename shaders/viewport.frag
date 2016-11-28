#version 410

in vec2 var_texcoords;

layout(location=0) out vec4 out_Color;

uniform sampler2D texmap;

void main()
{
  out_Color = vec4(texture(texmap, var_texcoords).xyz, 1);
}

