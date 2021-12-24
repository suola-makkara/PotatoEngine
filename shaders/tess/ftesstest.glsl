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
	vec3 l = normalize(vec3(1, 1, 1));
	vec3 n = normalize(normal);

	vec3 v = normalize(uCameraPos - pos.xyz);

	vec3 h = normalize(v + l);

	

	float d = length(lightPos - pos.xyz);

	vec3 green = vec3(0.8, 0.8, 0.82);
	vec3 gray = vec3(0.2, 0.2, 0.25);

	float amb = 0.2;
	vec3 diff = vec3(0.6, 0.6, 0.8);//(normal + 1) / 2;// vec3(pos.y / uHeightScale);//mix(gray, green, smoothstep(0.6, 0.9, n.y));
	float spec = dot(n, l) < 0 ? 0 : pow(max(0.0, dot(h, n)), 16.0);

	//fColor = vec4(color, 1);
	fColor = vec4(amb * diff + (0.1 * spec + diff) * max(0, dot(n, l)), 1);
}