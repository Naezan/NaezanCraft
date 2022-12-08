#version 330

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec2 inTextureCoord;

out vec2 vsTexCoord;

uniform mat4 model;
uniform mat4 projectionview;
uniform float dayTime;

mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

void main(){
    gl_Position = projectionview * model * rotationX(dayTime) * vec4(inVertexPosition, 1.0);

    vsTexCoord = vec2(inTextureCoord.x, inTextureCoord.y);
}