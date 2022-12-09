#version 330 core

in  vec2 vsTexCoord;

out vec4 outColor;

uniform sampler2D sunMoonTexture;

void main()
{
    vec4 color = texture(sunMoonTexture, vsTexCoord);
    
    outColor = color * 1.01;
}
