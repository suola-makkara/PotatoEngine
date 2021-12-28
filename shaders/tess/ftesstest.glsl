#version 460 core

out vec4 fColor;

in vec4 pos;
in vec3 normal;
in vec3 color;

uniform vec3 uCameraPos;
uniform float uHeightScale;

const vec3 lightPos = vec3(0,4,0);

void main()
{
	vec3 l = normalize(vec3(0.2, 1, 0.4));
	vec3 n = normalize(normal);

	vec3 v = normalize(uCameraPos - pos.xyz);

	vec3 h = normalize(v + l);

	float d = length(lightPos - pos.xyz);


	vec3 yy = vec3(0.8, 0.8, 0.6);
	vec3 red = vec3(0.8, 0.2, 0.2);
	vec3 orange = vec3(0.8, 0.4, 0.2);
	vec3 xx = vec3(0.2, 0.3, 0.5);

	vec3 diff = mix(xx, mix(orange, mix(red, yy, smoothstep(5, 15, pos.y)), smoothstep(-10, 10, pos.y)), smoothstep(-20, -5, pos.y));


	float amb = 0.2;
	float spec = dot(n, l) < 0 ? 0 : pow(max(0.0, dot(h, n)), 16.0);

	fColor = vec4(amb * diff + (0.1 * spec + diff) * max(0, dot(n, l)), 1);

	//fColor = vec4(vec3(0.8, 0.5,  1.0);
	//fColor = vec4((normal + 1.0) / 2.0, 1.0);
}