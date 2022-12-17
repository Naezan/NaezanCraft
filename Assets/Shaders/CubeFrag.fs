#version 330 core

// Input data
in vec2 TexCoord;
in float Light;

// Ouput data
out vec4 Outcolor;

uniform sampler2D cubeTexture;

void main()
{
	Outcolor = texelFetch(cubeTexture, ivec2(TexCoord.xy), 0);
	Outcolor = vec4(Outcolor.rgb * Light, Outcolor.a);
}