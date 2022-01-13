#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 normal;

uniform mat4 uProjView;

void main()
{
	normal =  normalize(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
		gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));

	gl_Position = uProjView * gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = uProjView * gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = uProjView * gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}