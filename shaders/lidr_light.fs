#version 410

in vec3 var_Normal;
in vec2 var_texcoords;
in vec3 var_Position;
in vec4 var_projectSpace;
//in vec4 var_shadowCoords;

layout(location=0) out vec4 out_Color;

//Material Supplied
uniform sampler2D texture_obj; //0
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;

//Camera Supplied
uniform vec3 camPos;
uniform vec2 screendims;

//lidr supplied
uniform sampler2D lightindex_tex;     //2
uniform sampler1D lightambient_tex;   //3
uniform sampler1D lightdiffuse_tex;   //4
uniform sampler1D lightspecular_tex;  //5
uniform sampler1D lightposition_tex;  //6


//Light Supplied
//uniform vec4 lightPos;
//uniform vec3 lightAmbient;
//uniform vec3 lightDiffuse;
//uniform vec4 lightSpecular;
//uniform vec3 lightConeDirection;
//uniform float far_plane;

#define NUM_LIGHTS 256.0
vec4 unpacklights(vec4 packedLight)
{
  vec4 unpackConst = vec4(4.0, 16.0, 64.0 , 256.0) / NUM_LIGHTS;
  vec4 floorValues = ceil(packedLight * 254.5);
  vec4 lightIndex;
  for (int i=0; i<4; i++)
  {
    packedLight = floorValues * 0.25;
    floorValues = floor(packedLight);
    lightIndex[i]=dot(packedLight - floorValues, unpackConst);
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

  //vec2 texcoords = (gl_FragCoord.xy/screendims.xy * 2.0f) - 1.0f;
  //vec4 light_indices = unpacklights(texture(lightindex_tex, texcoords));
  vec4 light_indices = unpacklights(textureProj(lightindex_tex, var_projectSpace));

  out_Color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  for (int i=0; i<4; i++)
  {
    float index = light_indices[i];
    if (index==0.0f)
      {
        out_Color.xyz += vec3(0.0f, 0.0f, 0.0f);
      }
    else
      {
        vec3 lightPos = texture(lightposition_tex, index).xyz;
        vec3 L = normalize(lightPos.xyz);
        vec3 V = normalize(camPos - pos_world.xyz);
        vec3 N = normalize(var_Normal);

        vec3 lightAmbient = texture(lightambient_tex, index).xyz;
        vec3 Iamb = ambient(lightAmbient);

        vec3 lightDiffuse = texture(lightdiffuse_tex, index).xyz;
        vec3 Idif = diffuse(N, L, lightDiffuse);

        vec3 lightSpecular = texture(lightspecular_tex, index).xyz;
        vec3 Ispe = specular(N, L, V, lightSpecular);

        vec3 diffuseColor = texture(texture_obj, var_texcoords).rgb;
        //out_Color.xyz += (Iamb * (Idif + Ispe)) * diffuseColor;
        out_Color.xyz  = diffuseColor;
      }
  }

//    vec3 L;
//    if (lightPos.w == 0) {
//      L = normalize(lightPos.xyz);
//    }
//    else {
//      L = normalize(lightPos.xyz - pos_world.xyz);
//    }
//    vec3 V = normalize(camPos - pos_world.xyz);
//    vec3 N = normalize(var_Normal);
//
//    vec3 coneDirection = normalize(lightConeDirection);
//    vec3 rayDirection = -L;
//    float att=1;
//    if (lightSpecular.a>0)
//    {
//      att = clamp(dot(rayDirection, coneDirection), 0.1f, 1.0f);
//    }
//
//    //vec3 Iamb = vec3(0.0f);
//    vec3 Idif = vec3(0.0f);
//    vec3 Ispe = vec3(0.0f);
//
//    vec3 Iamb = ambient();
//    Idif = diffuse(N, L);
//    Ispe = specular(N, L, V);
//    vec3 diffuseColor = texture(texture_obj, var_texcoords).rgb;
//
//    // combination of all components and diffuse color of the object
//    att=1.0f;
//    out_Color.xyz = (Iamb * (Idif + Ispe)) * diffuseColor * att;
//    //out_Color.a = 1.0f;
}


