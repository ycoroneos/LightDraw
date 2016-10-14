#version 410
in vec4 var_Color;

layout(location=0) out vec4 out_Color;


void main () {
    out_Color = var_Color;
}

