#version 330 core

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_TexCoord;
layout(location = 2) in float in_lightLevel;//LightLevel is 0~15(1byte~2byte for objectlight)
layout(location = 3) in float in_AO;//0~3

out vec2 TexCoord;
out float Light;

uniform mat4 model;
uniform mat4 projectionview;
uniform vec4 ambientLight;
uniform float lightIntensity;

uniform vec2 animTexCoord;

const int SUN_LIGHT_VALUE = 15;
const float MIN_LIGHT_VALUE = 0.1f;
const float	LIGHT_COEF = 0.06f;
const int TEXTURE_SIZE = 16;

void main(){
    gl_Position = projectionview * model * vec4(in_Pos.xyz, 1.0);
    TexCoord = vec2(in_TexCoord.x + animTexCoord.x * TEXTURE_SIZE, in_TexCoord.y + animTexCoord.y * TEXTURE_SIZE);
    Light = in_lightLevel * lightIntensity;
    Light = (Light * LIGHT_COEF + MIN_LIGHT_VALUE) * (in_AO * 0.33);
    //Light = (Light * LIGHT_COEF + MIN_LIGHT_VALUE);
}

