#version 430 core

layout(location = 0) in vec2 vertexPosition;

out vec3 color;
in int gl_VertexID;

void main()
{
    gl_Position.xy = vertexPosition;
    gl_Position.z = 1.0;
    gl_Position.w = 1.0;

    color = vec3(0.2f,0.2f,0.8f);
    if(gl_VertexID == 0)
        color = vec3(1.0f,0.0f,0.0f);
}
