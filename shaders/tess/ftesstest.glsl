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

	float spec = (pow(max(0.0, dot(h, n)), 16.0)) * max(0, dot(n, l));

	float d = length(lightPos - pos.xyz);

	vec3 green = vec3(0.8, 0.8, 0.82);
	vec3 gray = vec3(0.2, 0.2, 0.25);



	vec3 diff = mix(gray, green, smoothstep(0.8, 0.9, n.y));
	fColor = vec4(0.2 * spec + diff * (0.2 + 0.7 * max(0, dot(n, l))), 1);
	//fColor = vec4(vec3(spec),1);
	//fColor = vec4((normalize(normal) + 1) / 2, 1);
}