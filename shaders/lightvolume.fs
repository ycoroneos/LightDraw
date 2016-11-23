#version 410

in vec2 var_texcoords;
in vec4 var_position_radius_screen;

layout(location=0) out vec4 out_Color;

uniform sampler2D depthmap;

//input from LIDR
uniform vec2 screenres;

vec2 screen2ndc(vec2 input)
{
  float x = (input.x / screenres.x / 2.0f) - 1.0f;
  float y = (input.y / screenres.y / 2.0f) - 1.0f;
  return vec2(x,y);
}

void main()
{
  out_Color = vec4(0.0f);
  float lightradius = var_position_radius_screen.w;

  //xyz lightpos
  vec3 lightpos = var_position_radius_screen.xyz;

  //xyz scenepos
  float scenedepth = texture(depthmap, var_texcoords).r;
  vec3 scenepos = vec3((var_texcoords*2.0f) - 1.0f, scenedepth);
  if (abs(scenepos.z - lightpos.z) < 0.1f)
  //if (length(lightpos.xy - scenepos.xy)<lightradius && length(lightpos.z - scenepos.z) < lightradius)
  {
    out_Color = vec4(1.0f, 0.0f, 0.0f, 0.0f);
  }
  else
  {
    discard;
  }
}

