#version 460 core

out vec4 fColor;

in vec4 pos;
in vec3 normal;

uniform vec3 uCameraPos;

const vec3 lightPos = vec3(0,4,0);

void main()
{
	vec3 l = normalize(vec3(1, 1, 1));
	vec3 n = normalize(normal);

	vec3 v = normalize(uCameraPos - pos.xyz);

	vec3 h = normalize(v + l);

	float spec = dot(n, l) > 0.0 ? pow(max(0.0, dot(h, l)), 128.0) : 0.0;

	float d = length(lightPos - pos.xyz);

	fColor = vec4(0.2 + (0.2 * spec + 0.6 * vec3(max(0, dot(n, l)))), 1);
}