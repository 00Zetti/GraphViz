#version 430 core

layout(location = 0) in vec2 vertexPosition;

void main()
{
    gl_Position.xy = vertexPosition;
    gl_Position.z = 0.0f;
}
