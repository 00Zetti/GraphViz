#version 430 core

layout(location = 0) in vec2 vertexPosition;

out vec3 color;

void main()
{

    gl_Position.xy = vertexPosition;
    gl_Position.z = 1.0;
    gl_Position.w = 1.0;
    color = vec3(0.5f,0.5f,0.5f);
}
