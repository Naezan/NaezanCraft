#version 330 core

in  vec2 vsTexCoord;

out vec4 FragColor;

uniform sampler2D quadTexture;

void main()
{
	//texelFetch(quadTexture, ivec2(vsTexCoord.xy), 0);
	//vec4 color = texelFetch(quadTexture, ivec2(vsTexCoord.xy), 0);
	FragColor = texelFetch(quadTexture, ivec2(vsTexCoord.xy), 0);
	if(FragColor.a == 0) discard;
}