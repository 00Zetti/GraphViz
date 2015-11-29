#version 430 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec4 controlPoints;
layout(location = 2) in float t;
layout(location = 3) in float opacity;

uniform float beta;
uniform float maxLength;
uniform float minAlpha;
uniform float maxAlpha;

out vec4 color;

void main()
{

    //first and last controlpoint of the spline
    vec2 first = controlPoints.xy;
    vec2 last = controlPoints.zw;
    //use of beta parameter, see paper
    gl_Position.xy = vertexPosition * beta + (1.0f-beta) * (first + t * (last - first));
    gl_Position.z = 1.0;
    gl_Position.w = 1.0;

    //blend from red to green
    vec4 red = vec4(1.0f,0.0f,0.0f,1.0f);
    vec4 green = vec4(0.0f,1.0f,0.0f,1.0f);
    color = mix(red,green,t);

    //calc alpha value according to length of curve
    color.w = minAlpha * ((opacity-3.0f)/(11.0f-3.0f)) + maxAlpha * (1.0f-((opacity-3.0f)/(11.0f-3.0f)));

}
