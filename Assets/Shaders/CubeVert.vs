#version 330 core

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_TexCoord;

out vec2 out_TexCoord;

uniform mat4 model;
uniform mat4 projectionview;

void main(){

    gl_Position = projectionview * model * vec4(in_Pos.xyz, 1.0);
    out_TexCoord = vec2(in_TexCoord.x, in_TexCoord.y);
}

