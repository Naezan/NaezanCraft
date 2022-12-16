#version 330 core

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_TexCoord;
layout(location = 2) in uint ambientOcclusion;
layout(location = 3) in uint sunLight;

out vec2 TexCoord;
out float DirLight;
//out vec4 LightColor;

uniform mat4 model;
uniform mat4 projectionview;
uniform vec4 ambientLight;
uniform float lightIntensity;

const int SUN_LIGHT_VALUE = 15;
const float MIN_LIGHT_VALUE = 0.1f;
const float	LIGHT_COEF = 0.06f;

void main(){

    //float blockLight = ambientOcclusion < 16.f ? float(ambientOcclusion) : 10.f; 
    //blockLight /= 10.f;

    gl_Position = projectionview * model * vec4(in_Pos.xyz, 1.0);
    TexCoord = vec2(in_TexCoord.x, in_TexCoord.y);
    //DirLight = sunLight * lightIntensity;
    DirLight = SUN_LIGHT_VALUE * lightIntensity;
    DirLight = (DirLight * LIGHT_COEF + MIN_LIGHT_VALUE);
    //LightColor = ambientLight;
    //LightColor *= vec4(blockLight, blockLight, blockLight, 1.0f);
}

