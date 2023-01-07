#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 projectionview;

void main()
{
    gl_Position = projectionview * model * vec4(position.x, position.y, position.z, 1.0);
}