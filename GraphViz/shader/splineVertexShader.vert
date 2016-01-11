#version 130

in vec2 vertexPosition;
in vec4 controlPoints;
in float t;
in float opacity;

uniform float beta;
uniform float maxLength;
uniform float minAlpha;
uniform float maxAlpha;
uniform float splineIndex;

uniform vec4 mousePosition;
out vec4 color;
out float draw;

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
    vec4 blue = vec4(0.0f,0.0f,1.0f,1.0f);
    color = mix(red,green,t);

    float minX = mousePosition.x < mousePosition.y ? mousePosition.x : mousePosition.y;
    float maxX = mousePosition.x > mousePosition.y ? mousePosition.x : mousePosition.y;
    float minY = mousePosition.z < mousePosition.w ? mousePosition.z : mousePosition.w;
    float maxY = mousePosition.z > mousePosition.w ? mousePosition.z : mousePosition.w;
    if(gl_Position.x >= minX && gl_Position.x <= maxX && gl_Position.y >= minY && gl_Position.y <= maxY)
    {
        draw = 1.0f;
    }
    else
    {
        draw = 0.0f;
    }
    //calc alpha value according to length of curve
    color.w = minAlpha * ((opacity-3.0f)/(11.0f-3.0f)) + maxAlpha * (1.0f-((opacity-3.0f)/(11.0f-3.0f)));

}
