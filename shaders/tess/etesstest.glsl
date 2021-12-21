#version 460 core

layout (triangles, equal_spacing, ccw) in;

uniform mat4 uProj;

void main()
{
	vec4 pos = gl_TessCoord.x * gl_in[0].gl_Position
		+ gl_TessCoord.y * gl_in[1].gl_Position
		+ gl_TessCoord.z * gl_in[2].gl_Position;

	gl_Position = uProj * pos;
}