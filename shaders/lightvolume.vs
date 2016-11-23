#version 410

//https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/


out vec2 var_texcoords;
out vec4 var_position_radius_screen;


//input from scene graph
uniform vec4 light_position_radius;


//input from camera
uniform mat4 P;
uniform mat4 V;



void main ()
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    var_texcoords.x = (x+1.0)*0.5;
    var_texcoords.y = (y+1.0)*0.5;

    vec4 screenpos_center = P*V*vec4(light_position_radius.xyz, 1.0f);
    screenpos_center = screenpos_center/screenpos_center.w;



    vec4 screenpos_edge = P*V*vec4(vec3(light_position_radius.x+light_position_radius.w, light_position_radius.yz), 1.0f);
    screenpos_edge = screenpos_edge/screenpos_edge.w;
    float screenradius = length(screenpos_edge.xyz - screenpos_center.xyz);
    var_position_radius_screen = vec4(screenpos_center.xyz, screenradius);

    gl_Position = vec4(x, y, 0, 1);
}
