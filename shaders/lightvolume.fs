#version 410

in vec2 var_texcoords;
in vec4 var_position_radius_screen[127];
flat in int var_nlights;

layout(location=0) out vec4 out_Color;

uniform sampler2D depthmap;

void main()
{
  out_Color = vec4(0.0f);
  for (int i=0; i<var_nlights; i++)
  {
    float lightradius = var_position_radius_screen[i].w;
    vec2 lightpos = var_position_radius_screen[i].xy;
    float lightdepth = var_position_radius_screen[i].z;
    float scenedepth = texture(depthmap, var_texcoords).z;
    if (scenedepth > lightdepth && length(lightpos - gl_FragCoord.xy)<lightradius)
    {
      out_Color += vec4(0.1f, 0.0f, 0.0f, 0.0f);
    }
    else
    {
      out_Color += vec4(0.0f);
    }
  }
}

