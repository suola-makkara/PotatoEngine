#version 460 core

out vec4 fColor;

uniform vec4 uColor;

void main()
{
	if (gl_FrontFacing)
		fColor = uColor;
	else
		fColor = vec4(0.8 * uColor.xyz, uColor.w);
}