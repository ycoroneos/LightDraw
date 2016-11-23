#version 410

//https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/


out vec2 var_texcoords;


void main ()
{
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    var_texcoords.x = (x+1.0)*0.5;
    var_texcoords.y = (y+1.0)*0.5;

    gl_Position = vec4(x, y, 0, 1);
}
