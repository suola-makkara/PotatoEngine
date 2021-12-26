#version 460 core

layout (location = 0) in vec3 aPos;

uniform vec2 uChunkPos;

void main()
{
	gl_Position = vec4(aPos + vec3(uChunkPos.x, 0, uChunkPos.y), 1.0);
}