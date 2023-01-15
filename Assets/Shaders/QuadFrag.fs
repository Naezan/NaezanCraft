#version 330 core

in  vec2 vsTexCoord;

out vec4 FragColor;

uniform sampler2D quadTexture;

void main()
{
	vec4 color = texture(quadTexture, vsTexCoord);
	FragColor = color;
	if(FragColor.a == 0) discard;
}