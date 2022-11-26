#version 330 core

layout(location = 0) in vec3 in_Pos;

void main(){

    gl_Position = vec4(in_Pos.xyz, 1.0);
}

