#version 330 core

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_color;

out vec3 fragmentColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){

    gl_Position = projection * view * model * vec4(in_Pos.xyz, 1.0);
    fragmentColor = in_color;
}

