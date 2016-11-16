#version 410

in vec3 var_Normal;
in vec2 var_texcoords;
in vec3 var_Position;

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
uniform vec3 lightSpecular;

//vec3 blinn_phong(vec3 kd) {
//    vec4 pos_world = vec4(var_Position, 1);
//    vec3 normal_world = normalize(var_Normal);
//    pos_world /= pos_world.w;
//    vec3 light_dir = vec3(0,0,0);
//    if (lightPos.w == 0) {
//      light_dir = normalize(lightPos.xyz);
//    }
//    else {
//      light_dir = normalize(lightPos.xyz - pos_world.xyz);
//    }
//    vec3 cam_dir = camPos - pos_world.xyz;
//    cam_dir = normalize(cam_dir);
//
//    float ndotl = max(dot(normal_world, light_dir), 0.0);
//    vec3 diffContrib = lightDiffuse * kd * ndotl;
//
//    vec3 R = reflect( -light_dir, normal_world );
//    float eyedotr = max(dot(cam_dir, R), 0.0);
//    vec3 specContrib = pow(eyedotr, 3.0f) *
//                       1.0f * lightDiffuse;
//
//    return diffContrib + specContrib;
//    //return  + vec4(diffContrib + specContrib, alpha);
//}

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
   return matSpecular * lightSpecular * specularTerm;
}


void main () {
    vec4 pos_world = vec4(var_Position, 1);
    pos_world /= pos_world.w;
    vec3 L;
     if (lightPos.w == 0) {
       L = normalize(lightPos.xyz * -1);
     }
     else {
       L = normalize(lightPos.xyz - pos_world.xyz);
     }
    vec3 V = normalize(camPos - pos_world.xyz);
    vec3 N = normalize(var_Normal);

    // get Blinn-Phong reflectance components
    vec3 Iamb = ambient();
    vec3 Idif = diffuse(N, L);
    vec3 Ispe = specular(N, L, V);

    // diffuse color of the object from texture
    vec3 diffuseColor = texture(texture_obj, var_texcoords).rgb;

    // combination of all components and diffuse color of the object
    out_Color.xyz = diffuseColor * (Iamb + Idif + Ispe);
    out_Color.a = 1;
}


