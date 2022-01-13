#version 460 core

out vec4 fColor;

in vec3 normal;

uniform vec4 uColor;

void main()
{
	const vec3 l = normalize(vec3(1.0));

	if (gl_FrontFacing)
		fColor = vec4(uColor.xyz * (0.3 + max(0, dot(l, normalize(normal)))), uColor.w);
	else
		fColor = vec4(0.4 * uColor.xyz * (0.3 + max(0, dot(l, normalize(-normal)))), uColor.w);
}