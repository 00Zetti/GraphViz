#version 130


in vec3 color;
out vec4 fColor;

void main()
{
    fColor = vec4(color,0.5f);
}
