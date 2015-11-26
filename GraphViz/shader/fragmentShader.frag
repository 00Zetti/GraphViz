#version 400 core


in vec3 c;
out vec4 fColor;

void main()
{
    fColor = vec4(c,1.0f);
}
