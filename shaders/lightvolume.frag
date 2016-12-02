#version 410

in vec2 var_texcoords;

layout(location=0) out vec4 out_Color;

uniform sampler2D depthmap;
uniform sampler2D shadowmap;
uniform samplerCube cube_shadowmap;

//input from LIDR
//uniform vec2 screenres;

//input from camera
uniform mat4 PV_inverse;

//input from scene graph
uniform vec4 light_position_radius;
uniform vec4 light_cone_direction_angle;
uniform highp vec4 light_index;

//input from light
uniform mat4 light_PV;
uniform float shadows;
uniform float far_plane;

void main()
{
  out_Color = vec4(0.0f);

  //light radius in worldspace
  float radius = light_position_radius.w;

  //xyz lightpos in worldspace
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
  float angle = clamp(dot(normalize(L), D), 0, 1);

  //calculate shadows for spot light
  if (shadows>2.0f)
  {
    vec4 light_ndc = light_PV * vec4(scenepos_world_fixed, 1.0f);
    light_ndc = light_ndc/light_ndc.w;
    light_ndc = light_ndc*0.5f + 0.5f;
    float light_z = light_ndc.z;
    float light_depth = texture(shadowmap, light_ndc.xy).r;
    if (light_depth < (light_z - 0.02))
    {
      discard;
    }
  }
  //calculate shadows for point light
  else if (shadows>0.0f)
  {
    float light_depth = texture(cube_shadowmap, L).r * far_plane;
    if (light_depth < (length(L) - 0.02))
    {
      discard;
    }
  }

  if (acos(angle) < cone_angle && length(L) <= radius)
  {
    out_Color = light_index;
  }
  else
  {
    discard;
  }
}

