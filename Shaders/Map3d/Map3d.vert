#version 330 core
layout(location = 0) in float ht;

const int HTG_DIM_SIZE = 1201;
const float EARTH_RADIUS = 6378.0;
const float KM_SCALAR = 0.001;


uniform float lon;
uniform float lat;

uniform mat4 MVP;
    
out vec3 Color;

void main() {

    int position_i = gl_VertexID / HTG_DIM_SIZE;
    int position_j = gl_VertexID % HTG_DIM_SIZE;
    vec2 position = vec2(  lon + float(position_j) / float(HTG_DIM_SIZE - 1)  , lat - float(position_i) / float(HTG_DIM_SIZE - 1));

    float p_lon = radians(position.x);
    float p_lat = radians(position.y);

    vec3 earthPosition = vec3(
        (EARTH_RADIUS + ht*KM_SCALAR) * cos(p_lat) * cos(p_lon),
        (EARTH_RADIUS + ht*KM_SCALAR) * cos(p_lat) * sin(p_lon),
        (EARTH_RADIUS + ht*KM_SCALAR) * sin(p_lat));


    gl_Position = MVP * vec4(earthPosition, 1.0);


    if      (ht < 0  )   Color = vec3(0.,       0.,        1.); //blue
    else if (ht < 500)   Color = vec3(0.,       ht/500,    0.); //->green
    else if (ht < 1000)  Color = vec3(ht/500-1, 1.,        0.); //->yellow
    else if (ht < 2000)  Color = vec3(1.,       2.-ht/1000,0.); //->red
    else                 Color = vec3(1.,       ht/2000-1 ,ht/2000-1);  //->white
}