#version 410
layout(location=0) out vec4 out_color;
in vec2 texcoord;


uniform sampler2D t;
uniform float near_plane;
uniform float far_plane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}



void main()
{
   vec4 color1 = texture(t,texcoord);
   out_color = vec4(vec3(LinearizeDepth(color1.r) / far_plane), 1.0);
   //out_color = vec4(0.1f, 0.0f, 0.0f, 1.0f);
}

