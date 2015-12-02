#version 430 core

uniform sampler2D texture;

in vec4 color;
in float draw;
out vec4 fColor;

void main()
{
    ivec2 pixelCoords = ivec2(0,0);
   // vec3 value = texelFetch2D(texture,pixelCoords,0);

    if(draw > 0.5f)
    {
            fColor = color;
    }
    else
    {
        fColor = vec4(0.0f,0.0f,0.0f,0.0f);
    }
}
