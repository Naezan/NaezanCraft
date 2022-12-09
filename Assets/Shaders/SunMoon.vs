#version 330 core

layout(location = 0) in vec3 SunMoonVertexPosition;
layout(location = 1) in vec2 SunMoonTextureCoord;

out vec2 vsTexCoord;

uniform mat4 model;
uniform mat4 projectionview;
uniform float SunMoonTime;

mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

void main(){
    gl_Position = projectionview * model * vec4(SunMoonVertexPosition.xyz, 1.0);
    vsTexCoord = vec2(SunMoonTextureCoord.x, SunMoonTextureCoord.y);
}