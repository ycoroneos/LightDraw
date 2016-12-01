#version 410
//name=spotlight_shadow

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 Texcoord;


uniform mat4 M;
uniform mat4 light_PV;

void main()
{
  gl_Position =  light_PV * M * vec4(Position, 1.0f);
}



