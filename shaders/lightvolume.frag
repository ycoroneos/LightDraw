#version 410

in vec2 var_texcoords;

layout(location=0) out vec4 out_Color;

uniform sampler2D depthmap;

//input from LIDR
//uniform vec2 screenres;

//input from camera
uniform mat4 PV_inverse;

//input from scene graph
uniform vec4 light_position_radius;
uniform vec4 light_cone_direction_angle;
uniform highp vec4 light_index;

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

  //calculate fragment cone angle
  vec3 D = light_cone_direction_angle.xyz;
  vec3 L = scenepos_world_fixed - lightpos;
  float cone_angle = light_cone_direction_angle.w;
  float angle = clamp(dot(D, normalize(L)), 0, 1);

  if (angle < cone_angle && length(scenepos_world_fixed - lightpos) <= radius)
  {
    out_Color = light_index;
  }
  else
  {
    discard;
  }
}

