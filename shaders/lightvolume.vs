#version 410

//https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/


out vec2 var_texcoords;
//out vec4 var_light_worldpos;
//out float var_light_radius_view;


//input from camera
uniform mat4 P;
uniform mat4 V;



void main ()
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    var_texcoords.x = (x+1.0)*0.5;
    var_texcoords.y = (y+1.0)*0.5;

    //var_light_world = V*vec4(light_position_radius.xyz, 1.0f);
  //  var_light_worldpos = vec4(light_position_radius.xyz, 1.0f);



    //vec4 light_worldpos_edge = V*vec4(vec3(light_position_radius.x+light_position_radius.w, light_position_radius.yz), 1.0f);
    //var_light_radius_view = length(light_viewpos_edge - var_light_viewpos);

    gl_Position = vec4(x, y, 0, 1);
}
