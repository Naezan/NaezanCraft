#version 330 core

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_TexCoord;
layout(location = 2) in float in_lightLevel;//LightLevel is 0~15(1byte~2byte for objectlight)
//layout(location = 3) in uint in_AO;

out vec2 TexCoord;
out float Light;

uniform mat4 model;
uniform mat4 projectionview;
uniform vec4 ambientLight;
uniform float lightIntensity;

const int SUN_LIGHT_VALUE = 15;
const float MIN_LIGHT_VALUE = 0.1f;
const float	LIGHT_COEF = 0.06f;

void main(){
    gl_Position = projectionview * model * vec4(in_Pos.xyz, 1.0);
    TexCoord = vec2(in_TexCoord.x, in_TexCoord.y);
    Light = in_lightLevel * lightIntensity;
    //Light = (Light * LIGHT_COEF + MIN_LIGHT_VALUE) * in_AO;
    Light = (Light * LIGHT_COEF + MIN_LIGHT_VALUE);
}

