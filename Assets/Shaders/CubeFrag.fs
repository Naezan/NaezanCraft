#version 330 core

// Input data
in vec2 TexCoord;
in float DirLight;
//in vec4 LightColor;

// Ouput data
out vec4 OutCubecolor;

uniform sampler2D cubeTexture;

void main()
{
	OutCubecolor = texelFetch(cubeTexture, ivec2(TexCoord.xy), 0);
	OutCubecolor = vec4(OutCubecolor.rgb * DirLight, OutCubecolor.a);
	//color *= LightColor;
}