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

//AnimTexture
uniform vec2 animTexCoord;

//WaterWave
uniform float waveTime;

//rvalue
const float PI = 3.1415926535897932384626433832795;
const float waveLength = 4.0;
const float waveAmplitude = 0.2;

const int SUN_LIGHT_VALUE = 15;
const float MIN_LIGHT_VALUE = 0.1f;
const float	LIGHT_COEF = 0.06f;

float generateOffset(float x, float z, float val1, float val2){
	float radiansX = ((mod(x+z*x*val1, waveLength)/waveLength) + waveTime * mod(x * 0.8 + z, 1.5)) * 2.0 * PI;
	float radiansZ = ((mod(val2 * (z*x +x*z), waveLength)/waveLength) + waveTime * 2.0 * mod(x , 2.0) ) * 2.0 * PI;
	return waveAmplitude * 0.5 * (sin(radiansZ) + cos(radiansX));
}

vec3 applyDistortion(vec3 vertex){
	//float xDistortion = generateOffset(vertex.x, vertex.z, 0.2, 0.1);
	float yDistortion = generateOffset(vertex.x, vertex.z, 0.1, 0.3);
	//float zDistortion = generateOffset(vertex.x, vertex.z, 0.15, 0.2);
	return vertex + vec3(0, yDistortion, 0);
}

void main(){

    vec3 currentVertex = vec3(in_Pos.x, in_Pos.y, in_Pos.z);
    currentVertex = applyDistortion(currentVertex);

    gl_Position = projectionview * model * vec4(currentVertex.xyz, 1.0);
    TexCoord = vec2(in_TexCoord.x + animTexCoord.x, in_TexCoord.y + animTexCoord.y);
    Light = in_lightLevel * lightIntensity;
    Light = (Light * LIGHT_COEF + MIN_LIGHT_VALUE) * (in_AO * 0.33);
}

