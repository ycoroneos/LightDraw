#version 410

layout(location=0) out float frag_depth;


void main () {
    frag_depth = gl_FragCoord.z;
    //frag_depth=1;
}



