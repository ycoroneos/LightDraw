#version 410

in vec3 var_Normal;
in vec2 var_texcoords;
in vec3 var_Position;
in vec4 var_projectSpace;

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

#define NUM_LIGHTS 256.0
highp vec4 unpacklights(vec4 packedLight)
{
  highp vec4 unpackConst = vec4(4.0, 16.0, 64.0 , 256.0) / NUM_LIGHTS;
  highp vec4 floorValues = ceil(packedLight * 254.5);
  highp vec4 lightIndex;
  for (int i=0; i<4; i++)
  {
    packedLight = floorValues * 0.25;
    floorValues = floor(packedLight);
    highp vec4 fracpart = packedLight - floorValues;
    lightIndex[i]=dot(fracpart, unpackConst);
  }
  return lightIndex;
}

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

  highp vec4 light_indices = unpacklights(textureProj(lightindex_tex, var_projectSpace));

  out_Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  for (int i=0; i<4; i++)
  {
    highp float index = light_indices[i];
    vec3 diffuseColor = texture(texture_obj, var_texcoords).rgb;
    if (index==0.0f)
      {
        out_Color.xyz = diffuseColor / 10.0f;
      }
    else
      {
        vec4 lightPos_att = texture(lightposition_tex, index).xyzw;
        vec3 lightPos = lightPos_att.xyz;
        float att = lightPos_att.w;
        vec3 L = normalize(lightPos.xyz - pos_world.xyz);
        vec3 V = normalize(camPos - pos_world.xyz);
        vec3 N = normalize(var_Normal);

        vec3 lightAmbient = texture(lightambient_tex, index).xyz;
        vec3 Iamb = ambient(lightAmbient);

        vec3 lightDiffuse = texture(lightdiffuse_tex, index).xyz;
        vec3 Idif = diffuse(N, L, lightDiffuse);

        vec3 lightSpecular = texture(lightspecular_tex, index).xyz;
        vec3 Ispe = specular(N, L, V, lightSpecular);

        float distance = 1.0f / length(lightPos - pos_world.xyz);
        out_Color.xyz += (Iamb * (Idif + Ispe)) * diffuseColor * distance * distance * att;
      }
  }

}


