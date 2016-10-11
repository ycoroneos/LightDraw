#version 410
layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec3 Color;
layout(location=3) in mat4 InstanceTransform;

out vec4 var_Color;

uniform mat4 P;
uniform mat4 V;

void main () {
    gl_Position = P * V * InstanceTransform * vec4(Position, 1);
    var_Color = vec4(Color,1.0);
}