#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextureCoord;

out vec2 vsTexCoord;

uniform mat4 model;
uniform mat4 projection;

void main(){
    gl_Position = projection * model * vec4(aPos.xyz, 1.0);
    vsTexCoord = vec2(aTextureCoord.x, aTextureCoord.y);
}

