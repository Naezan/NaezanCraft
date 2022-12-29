#version 330

layout(location = 0) in vec3 in_Pos;

uniform mat4 model;
uniform mat4 projectionview;

void main()
{
    gl_Position = projViewMatrix * modelMatrix * vec4(in_Pos, 1.0);
}
