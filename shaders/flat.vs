#version 410
layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 Texcoord;

out vec4 var_Color;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main () {
    gl_Position = P * V * M * vec4(Position, 1);
    vec4 ncolor = vec4(0.2f, 0.3f, 0.2f, 1.0);
    normalize(ncolor);
    var_Color = ncolor;
}
