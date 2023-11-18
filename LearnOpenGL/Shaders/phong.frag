#version 330 core

// Helper functions

#define GAMMA 2.2
#define PI 3.1415926535897932384626433832795028841972

// _Time - Time since level load (t/20, t, t*2, t*3), use to animate things inside the shaders.
// _SinTime _CosTime = (t/8, t/4, t/2, t).
// unity_DeltaTime = Delta time: (dt, 1/dt, smoothDt, 1/smoothDt)
uniform vec4 _Time;

// https://www.shadertoy.com/view/lscSzl
vec3 encodeSRGB(vec3 linearRGB) {
	vec3 a = 12.92 * linearRGB;
	vec3 b = 1.055 * pow(linearRGB, vec3(1.0 / 2.4)) - 0.055;
	vec3 c = step(vec3(0.0031308), linearRGB);
	return mix(a, b, c);
}

vec3 decodeSRGB(vec3 screenRGB) {
	vec3 a = screenRGB / 12.92;
	vec3 b = pow((screenRGB + 0.055) / 1.055, vec3(2.4));
	vec3 c = step(vec3(0.04045), screenRGB);
	return mix(a, b, c);
}

vec3 gamma(vec3 color, float g) {
	return pow(color, vec3(g));
}

vec3 exposureMapping(vec3 color, float exposure) {
	return vec3(1.0) - exp(-color * exposure);
}

// Material varying/uniforms

in vs_out {
	vec3 WSPosition;
	vec2 TexCoords;
	vec3 Normal;
	mat3 TBN;
} varyings;

uniform vec3 cameraPos;
uniform float exposure;

out vec4 FragColor;

struct Material {
	uint maps;
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
	sampler2D texture_normal0;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

// Surface data holder

const uint cDiffuse = 0x00000001u;
const uint cSpecular = 0x00000002u;
const uint cNormal = 0x00000004u;
#define MAP_DEFINDED(C) (material.maps & C) != 0u

struct Surface {
	vec3 pos;
	vec3 normal;
	vec3 viewDir;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

Surface getSurface(Material material) {
	Surface surface;
	surface.pos = varyings.WSPosition;
	surface.viewDir = normalize(cameraPos - varyings.WSPosition);

	if(MAP_DEFINDED(cDiffuse)) {
		surface.diffuse = decodeSRGB(texture(material.texture_diffuse0, varyings.TexCoords).rgb);
	} else {
		surface.diffuse = material.diffuse;
	}

	if(MAP_DEFINDED(cSpecular)) {
		surface.specular = decodeSRGB(texture(material.texture_specular0, varyings.TexCoords).rgb);
	} else {
		surface.specular = material.specular;
	}

	if(MAP_DEFINDED(cNormal)) {
		surface.normal = texture(material.texture_normal0, varyings.TexCoords).rgb * 2.0 - 1.0;
		surface.normal = normalize(varyings.TBN * surface.normal);
	} else {
		surface.normal = normalize(varyings.Normal);
	}

	surface.shininess = material.shininess;
	return surface;
}

// Light calculation.

#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_OTHER_LIGHTS 32
#define SQUER_FALLOF_WINDOWING // SQUER_FALLOF
#define BLIN_PHONG

struct DirLight {
	vec3 dir;
	vec3 color;
};

struct OtherLight {
	vec4 pos;
	vec3 color;
	vec3 dir;
	vec2 spotAngles;
};

struct Light {
	vec3 dir;
	vec3 color;
	float attenuation;
};

layout(std140) uniform Lights {
	uniform int directionalLightNum;
	uniform int otherLightNum;
	uniform DirLight directionalLight[MAX_DIRECTIONAL_LIGHTS];
	uniform OtherLight otherLights[MAX_OTHER_LIGHTS];
};

Light getDirLight(int index) {
	Light light;
	light.dir = -directionalLight[index].dir;
	light.color = directionalLight[index].color;
	light.attenuation = 1.0;
	return light;
}

float getAttenuation(int index, vec3 posToLight) {
#if defined(SQUER_FALLOF)
	float dist = length(posToLight);
	float I = otherLights[index].pos.w;
	return I / (4 * PI * dist * dist);
#elif defined(SQUER_FALLOF_WINDOWING)
	float lightInvRadius = otherLights[index].pos.w;
	float distanceSquare = dot(posToLight, posToLight);
	float factor = distanceSquare * lightInvRadius * lightInvRadius;
	float smoothFactor = max(1.0 - factor * factor, 0.0);
	return (smoothFactor * smoothFactor) / max(distanceSquare, 1e-4);
#endif
}

float getSpotAngleAtteuation(int index, vec3 dirToLight) {
	vec2 spot = otherLights[index].spotAngles;
	vec3 spotDir = -normalize(otherLights[index].dir);
	return clamp(dot(spotDir, dirToLight) * spot.x + spot.y, 0.0, 1.0);
}

Light getOtherLight(int index, Surface surface) {
	vec3 posToLight = vec3(otherLights[index].pos) - surface.pos;
	vec3 dirToLight = normalize(posToLight);

	float atten = getAttenuation(index, posToLight);
	float attenAngle = getSpotAngleAtteuation(index, dirToLight);

	Light light;
	light.dir = dirToLight;
	light.color = otherLights[index].color;
	light.attenuation = atten * attenAngle;
	return light;
}

vec3 calculateLight(Surface surface, Light light) {
	vec3 reflectDir = reflect(-light.dir, surface.normal);
	vec3 n = surface.normal;
	vec3 l = light.dir;

	float NoL = clamp(dot(n, l), 0.0, 1.0);
	vec3 diffuse = NoL * surface.diffuse;

	float s = surface.shininess;
#if defined(BLIN_PHONG)
	// https://www.rorydriscoll.com/2009/01/25/energy-conservation-in-games/
	float kEnergyConservation = (8.0 + s) / (8.0 * PI);
	float NoH = dot(n, normalize(surface.viewDir + light.dir));
	float spec = kEnergyConservation * pow(clamp(NoH, 0.0, 1.0), surface.shininess);
#elif defined(PHONG)
	float kEnergyConservation = (2.0 + s) / (2.0 * PI);
	float RoV = dot(reflectDir, surface.viewDir);
	float spec = kEnergyConservation * pow(clamp(RoV, 0.0, 1.0), s);
#endif
	vec3 specular = spec * surface.specular;

	return (diffuse + specular) * light.color * light.attenuation;
}

void main() {
	Surface surface = getSurface(material);

	vec3 color;
	for(int i = 0; i < directionalLightNum; i++) {
		color += calculateLight(surface, getDirLight(i));
	}

	for(int i = 0; i < otherLightNum; i++) {
		color += calculateLight(surface, getOtherLight(i, surface));
	}

	color = exposureMapping(color, exposure);
	color = encodeSRGB(color);
	FragColor = vec4(color, 1.0);
}
