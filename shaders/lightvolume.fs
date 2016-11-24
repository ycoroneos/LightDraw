#version 410

in vec2 var_texcoords;
//in vec4 var_light_viewpos;
//in float var_light_radius_view;

layout(location=0) out vec4 out_Color;

uniform sampler2D depthmap;

//input from LIDR
uniform vec2 screenres;

//input from camera
uniform mat4 PV_inverse;

//input from scene graph
uniform vec4 light_position_radius;

vec2 screen2ndc(vec2 input)
{
  float x = (input.x / screenres.x / 2.0f) - 1.0f;
  float y = (input.y / screenres.y / 2.0f) - 1.0f;
  return vec2(x,y);
}

void main()
{
  out_Color = vec4(0.0f);

  //light radius in worldspace
  float radius = light_position_radius.w;

  //xyz lightpos in viewspace
  vec3 lightpos = light_position_radius.xyz;


  //xyz scenepos in clip space
  float scenedepth = texture(depthmap, var_texcoords).r * 2.0f - 1.0f;
  vec4 scenepos_clip = vec4((var_texcoords*2.0f) - 1.0f, scenedepth, 1.0f);
  vec4 scenepos_world = PV_inverse * scenepos_clip;
  vec3 scenepos_world_fixed = scenepos_world.xyz/scenepos_world.w;

  if (length(scenepos_world_fixed - lightpos) < radius)
  {
    out_Color = vec4(1.0f, 0.0f, 0.0f, 0.0f);
  }
  else
  {
    discard;
  }
}

