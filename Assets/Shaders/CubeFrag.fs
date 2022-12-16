#version 330 core

// Input data
in vec2 TexCoord;
in float DirLight;
//in vec4 LightColor;

// Ouput data
out vec4 color;

uniform sampler2D cubeTexture;

void main()
{
	color = texelFetch(cubeTexture, ivec2(TexCoord.xy), 0);
	color = vec4(color.rgb * DirLight, color.a);
	//color *= LightColor;
}