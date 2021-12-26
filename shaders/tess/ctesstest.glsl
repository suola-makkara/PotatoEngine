#version 460 core

layout (vertices = 3) out;

uniform vec3 uCameraPos;

void main()
{
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz;

	vec3 m0 = 0.5 * (p1 + p2);
	vec3 m1 = 0.5 * (p2 + p0);
	vec3 m2 = 0.5 * (p0 + p1);

	float d0 = 50.0 / (1.0 + length(uCameraPos - m0));
	float d1 = 50.0 / (1.0 + length(uCameraPos - m1));
	float d2 = 50.0 / (1.0 + length(uCameraPos - m2));

	gl_TessLevelOuter[0] = d0;
	gl_TessLevelOuter[1] = d1;
	gl_TessLevelOuter[2] = d2;

	gl_TessLevelInner[0] = 1.0 / 3.0 * (d0 + d1 + d2);

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}