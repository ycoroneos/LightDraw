#version 410

in vec3 var_Normal;
in vec2 var_texcoords;
in vec3 var_Position;

layout(location=0) out vec4 out_Color;

//Material Supplied
uniform sampler2D texture_obj; //0
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;

//Camera Supplied
uniform vec3 camPos;

//lidr supplied
uniform sampler2D lightindex_tex;     //2
uniform sampler1D lightambient_tex;   //3
uniform sampler1D lightdiffuse_tex;   //4
uniform sampler1D lightspecular_tex;  //5
uniform sampler1D lightposition_tex;  //6

//light supplied
uniform vec4 lightPos_att;
uniform vec4 lightCone_direction_angle;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

//light supplied for shadows
uniform sampler2D shadowmap;
uniform samplerCube cube_shadowmap;
uniform mat4 light_PV;
uniform float shadows;
uniform float far_plane;


vec3 ambient(vec3 lightAmbient)
{
  return matAmbient * lightAmbient;
}

vec3 diffuse(vec3 N, vec3 L, vec3 lightDiffuse)
{
  float diffuseTerm = clamp(dot(N, L), 0, 1) ;
  return matDiffuse * lightDiffuse * diffuseTerm;
}

vec3 specular(vec3 N, vec3 L, vec3 V, vec3 lightSpecular)
{
   float specularTerm = 0;

   if(dot(N, L) > 0)
   {
      vec3 H = normalize(L + V);
      specularTerm = pow(dot(N, H), matShininess);
   }
   return matSpecular * vec3(lightSpecular) * specularTerm;
}

void main () {
  vec4 pos_world = vec4(var_Position, 1);
  pos_world /= pos_world.w;


  out_Color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  vec3 lightPos = lightPos_att.xyz;


  //some variables we will always use
  float radius = lightPos_att.w;
  float distance = length(lightPos - pos_world.xyz);
  vec3 diffuseColor = texture(texture_obj, var_texcoords).rgb;
  vec3 D = lightCone_direction_angle.xyz;
  float cone_angle = lightCone_direction_angle.w;
  float angle = clamp(dot(normalize(pos_world.xyz - lightPos), D), 0, 1);

  //first check if fragment is inside the light volume
  if (distance < radius && acos(angle) < cone_angle)
  {
    //calculate shadows for spot light
    if (shadows>2.0f)
    {
      vec4 light_ndc = light_PV * pos_world;
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
      vec3 lookup = pos_world.xyz - lightPos;
      float light_depth = texture(cube_shadowmap, lookup).r * far_plane;
      if (light_depth < (length(lookup) - 0.02))
      {
        discard;
      }
    }

    float att = lightPos_att.w;
    vec3 L = normalize(lightPos.xyz - pos_world.xyz);
    vec3 V = normalize(camPos - pos_world.xyz);
    vec3 N = normalize(var_Normal);

    vec3 Iamb = ambient(lightAmbient);

    vec3 Idif = diffuse(N, L, lightDiffuse);

    vec3 Ispe = specular(N, L, V, lightSpecular);

    float distance_att = 1.0f / distance;
    out_Color.xyz = (Iamb * (Idif + Ispe)) * diffuseColor * distance_att * distance_att * att;
  }
  else
  {
    out_Color.xyz = diffuseColor / 10.0f;
  }
}


