#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 uProjView;
uniform vec3 uPosition;

void main()
{
	gl_Position = uProjView * vec4(aPos + uPosition, 1.0);
}