#version 330 core

layout(location = 0) in vec3 CloudVertexPosition;
layout(location = 1) in vec2 CloudTexCoord;

out vec2 TexCoord;
out float Light;

uniform mat4 model;
uniform mat4 projectionview;
uniform float lightIntensity;

const float	LIGHT_COEF = 0.8;

void main(){
    gl_Position = projectionview * model * vec4(CloudVertexPosition.xyz, 1.0);
    Light = lightIntensity * LIGHT_COEF;
    TexCoord = vec2(CloudTexCoord.x, CloudTexCoord.y);
}

