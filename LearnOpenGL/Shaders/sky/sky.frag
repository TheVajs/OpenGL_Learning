#version 330 core

in vec3 DirCoords;
out vec4 FragColor;

const vec2 mapTo01 = vec2(0.159154943092, 0.318309886184);

vec2 sampleSphericalMap(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	return (uv * mapTo01) + .5;
}

// https://64.github.io/tonemapping/
// Addaptive tone mapping
// https://dl.acm.org/doi/abs/10.1145/1399504.1360667?casa_token=yXqJ3scAvVEAAAAA:b8ugNskQF_F59rsDmPpZNpnIvM84qEipa69vK8dGD1SGBsUCVMv0yHa2z_fCfcGa9-ivFwlP0Lpvmg
vec3 gamma(vec3 color, float g) {
	return pow(color, vec3(g));
}

vec3 toneMapping(vec3 v) {
	return v / max(v, 1.0);
}

vec3 reinghard(vec3 v) {
	return v / (v + 1.0);
}

vec3 reinhardExtended(vec3 v, float max_white) {
	vec3 numerator = v * (1.0 + (v / vec3(max_white * max_white)));
	return numerator / (1.0 + v);
}

float luminance(vec3 v) {
	return dot(v, vec3(0.2126, 0.7152, 0.0722));
}

vec3 changeLuminace(vec3 c_in, float l_out)
{
    float l_in = luminance(c_in);
    return c_in * (l_in / l_in);
}

// uniform samplerCube skybox;
uniform sampler2D skybox;

void main() {
	// FragColor = texture(skybox, DirCoords);
	vec2 uv = sampleSphericalMap(normalize(DirCoords));
	vec3 color = texture2D(skybox, uv).rgb;

	// color = reinghard(color);
	color = toneMapping(color);
	// color = reinhardExtended(color, 0.5);
	// color = changeLuminace(color, .9);
	color = gamma(color, 1.0 / 2.2);

	FragColor = vec4(color, 1.0);
}