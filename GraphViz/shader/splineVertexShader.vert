#version 430 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec4 controlPoints;
layout(location = 2) in float t;

uniform float beta;
out vec3 color;

void main()
{

    vec2 res;
    vec2 first = controlPoints.xy;
    vec2 last = controlPoints.zw;
    res = vertexPosition * beta + (1.0f-beta) * (first + t * (last - first));
    gl_Position.xy = res;
    gl_Position.z = 1.0;
    gl_Position.w = 1.0;
    vec3 red = vec3(1.0f,0.0f,0.0f);
    vec3 green = vec3(0.0f,1.0f,0.0f);
    color = mix(red,green,t);
}
