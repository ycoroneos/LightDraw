#version 410

in vec2 var_texcoords;
in vec4 var_position_radius_screen;

layout(location=0) out vec4 out_Color;

uniform sampler2D depthmap;

void main()
{
    out_Color = vec4(0.0f);
    float lightradius = var_position_radius_screen.w;
    vec2 lightpos = var_position_radius_screen.xy;
    float lightdepth = var_position_radius_screen.z;
    float scenedepth = texture(depthmap, var_texcoords).r;
//    if (scenedepth > lightdepth && length(lightpos - gl_FragCoord.xy)<lightradius)
//    {
//      out_Color = vec4(0.1f, 0.0f, 0.0f, 0.0f);
//    }
    if (length(vec2(lightpos) - gl_FragCoord.xy)<lightradius && scenedepth>lightdepth && lightdepth>0.001f)
    {
      out_Color = vec4(1.0f, 0.0f, 0.0f, 0.0f);
    }
    else
    {
      discard;
    }
}

