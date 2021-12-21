#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uView;

void main()
{
	gl_Position = uView * vec4(aPos, 1.0);
}