#version 330 core

// Input data
in vec2 TexCoord;

// Ouput data
out vec4 color;

uniform sampler2D cubeTexture;

void main()
{
	color = texelFetch(cubeTexture, ivec2(TexCoord.xy), 0);
}