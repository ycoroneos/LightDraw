#version 410

in vec3 var_Normal;
in vec2 var_texcoords;
in vec3 var_Position;
//in vec4 var_shadowCoords;

layout(location=0) out vec4 out_Color;

//Material Supplied
uniform sampler2D texture_obj;
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;

//Camera Supplied
uniform vec3 camPos;

//Light Supplied
uniform vec4 lightPos;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec4 lightSpecular;
uniform vec3 lightConeDirection;
//uniform float far_plane;

vec3 ambient()
{
  return matAmbient * lightAmbient;
}

vec3 diffuse(vec3 N, vec3 L)
{
  float diffuseTerm = clamp(dot(N, L), 0, 1) ;
  return matDiffuse * lightDiffuse * diffuseTerm;
}

vec3 specular(vec3 N, vec3 L, vec3 V)
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
    vec3 L;
     if (lightPos.w == 0) {
       L = normalize(lightPos.xyz);
     }
     else {
       L = normalize(lightPos.xyz - pos_world.xyz);
     }
    vec3 V = normalize(camPos - pos_world.xyz);
    vec3 N = normalize(var_Normal);

    vec3 coneDirection = normalize(lightConeDirection);
    vec3 rayDirection = -L;
    float att=1;
    if (lightSpecular.a>0)
    {
      att = clamp(dot(rayDirection, coneDirection), 0.1f, 1.0f);
    }

    //vec3 Iamb = vec3(0.0f);
    vec3 Idif = vec3(0.0f);
    vec3 Ispe = vec3(0.0f);

    vec3 Iamb = ambient();
    Idif = diffuse(N, L);
    Ispe = specular(N, L, V);
    vec3 diffuseColor = texture(texture_obj, var_texcoords).rgb;

    // combination of all components and diffuse color of the object
    att=1.0f;
    out_Color.xyz = (Iamb * (Idif + Ispe)) * diffuseColor * att;
    //out_Color.a = 1.0f;
}


