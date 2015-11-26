#version 400 core

layout(location = 0) in vec2 vertexPosition;

in int gl_VertexID;
out vec3 c;
void main()
{

    gl_Position.xy = vertexPosition.xy;
    gl_Position.z = 1.0f;
    gl_Position.w = 1.0f;


    //first point( center) draw in blue
    if(gl_VertexID == 0)
    {
        c = vec3(0.0f,0.0f,1.0f);
    }
    else
    {
        c = vec3(1.0f,0.0f,0.0f);
    }
}
