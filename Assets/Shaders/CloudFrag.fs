#version 330 core

// Input data
in vec2 TexCoord;
in float Light;

// Ouput data
out vec4 Outcolor;

uniform sampler2D cloudTexture;

void main()
{
	vec4 color = texture(cloudTexture, TexCoord);
	Outcolor = vec4(color.rgb * Light, color.a * 0.5);
	if (Outcolor.a == 0) discard;
}