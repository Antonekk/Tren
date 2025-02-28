#version 330 core
layout(location = 0) in float ht;

const int HTG_DIM_SIZE = 1201;

uniform float zoom;
uniform vec2 offset;

uniform float lon;
uniform float lat;
uniform float step;
uniform float aspect;
    
out vec3 Color;

void main() {

    int position_i = gl_VertexID / HTG_DIM_SIZE;
    int position_j = gl_VertexID % HTG_DIM_SIZE;
    vec2 table_position = vec2(  lon + float(position_j) / float(HTG_DIM_SIZE - 1)  , lat - float(position_i) / float(HTG_DIM_SIZE - 1) );

    vec2 pos = (table_position- offset) * zoom;
    gl_Position = vec4(pos.x*aspect,pos.y,0.0, 1.0) ;

    if      (ht < 0  )   Color = vec3(0.,       0.,        1.); //blue
    else if (ht < 500)   Color = vec3(0.,       ht/500,    0.); //->green
    else if (ht < 1000)  Color = vec3(ht/500-1, 1.,        0.); //->yellow
    else if (ht < 2000)  Color = vec3(1.,       2.-ht/1000,0.); //->red
    else                 Color = vec3(1.,       ht/2000-1 ,ht/2000-1);  //->white
}