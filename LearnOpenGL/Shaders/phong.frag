#version 330 core

// Interface blocks, helps organize varying values.
in vs_out {
	vec3 Normal;
	vec3 WSPosition;
	vec2 TexCoords;
} varyings;
out vec4 FragColor;

uniform float uTime;
uniform vec3 cameraPos;

struct Material {
	bool light_maps;
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
	// http://devernay.free.fr/cours/opengl/materials.html (old vay to simulate materials)
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

// Surface data holder

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
	surface.normal = normalize(varyings.Normal);
	surface.viewDir = normalize(cameraPos - varyings.WSPosition);

	if (material.light_maps) {
		surface.diffuse = vec3(texture(material.texture_diffuse0, varyings.TexCoords));
		surface.specular = vec3(texture(material.texture_specular0, varyings.TexCoords));
	} else {
		surface.diffuse = material.diffuse;
		surface.specular = material.specular;
	}

	surface.shininess = material.shininess;
	return surface;
}

// Utility

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

// Light calculation.

#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_OTHER_LIGHTS 16

layout(std140) uniform DirLights {
	uniform int directionalLightNum;
	uniform vec3 directionalLightDirections[MAX_DIRECTIONAL_LIGHTS];
	uniform vec3 directionalLightColors[MAX_DIRECTIONAL_LIGHTS];
};

layout(std140) uniform OtherLights {
	uniform int otherLightNum;
	uniform vec4 otherLightPositions[MAX_OTHER_LIGHTS];
	uniform vec3 otherLightColors[MAX_OTHER_LIGHTS];
	uniform vec3 otherLightDirections[MAX_OTHER_LIGHTS];
	uniform vec2 otherLightAttenuations[MAX_OTHER_LIGHTS];
};

uniform vec3 ambient;

struct Light {
	vec3 dir;
	vec3 color;
	float attenuation;
};

float angleAttenuation(int index) {
	vec3 direction = otherLightDirections[index];
	vec2 angles = otherLightAttenuations[index];

	// TODO

	return 1.0;
}

Light GetDirLight(int index) {
	Light light;
	light.dir = directionalLightDirections[index];
	light.color = directionalLightColors[index];
	light.attenuation = 1.0;
	return light;
}

Light GetOtherLight(int index, Surface surface) {
	vec3 lightDir = otherLightPositions[index].xyz - surface.pos;

	Light light;
	light.dir = normalize(lightDir);
	light.color = otherLightColors[index];
	light.attenuation = otherLightPositions[index].w * angleAttenuation(index);
	return light;
}

vec3 calculateLight(Surface surface, Light light) {
	vec3 reflectDir = reflect(-light.dir, surface.normal);

	float diff = saturate(dot(surface.normal, light.dir));
	vec3 diffuse = diff * surface.diffuse;

	float spec = pow(saturate(dot(reflectDir, surface.viewDir)), surface.shininess);
	vec3 specular = spec * surface.specular;

	return (diffuse + specular) * light.color * light.attenuation;
}

void main() {
	Surface surface = getSurface(material);

	vec3 color;
	for(int i = 0; i < directionalLightNum; i++) {
		color += calculateLight(surface, GetDirLight(i));
	}

	for(int i = 0; i < otherLightNum; i++) {
		color += calculateLight(surface, GetOtherLight(i, surface));
	}

	FragColor = vec4(ambient + color, 1.0);
}
