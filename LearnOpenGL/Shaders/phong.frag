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
uniform vec3 ambient;

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

	if(material.light_maps) {
		surface.diffuse = vec3(texture(material.texture_diffuse0, varyings.TexCoords));
		surface.specular = vec3(texture(material.texture_specular0, varyings.TexCoords));
	} else {
		surface.diffuse = material.diffuse;
		surface.specular = material.specular;
	}

	surface.shininess = material.shininess;
	return surface;
}

// Light calculation.

#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_OTHER_LIGHTS 32

struct DirLight {
	vec3 dir;
	vec3 color;
};

struct OtherLight {
	vec4 pos;
	vec3 color;
	vec3 dir;
	vec2 angle;
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

Light GetDirLight(int index) {
	Light light;
	light.dir = -directionalLight[index].dir;
	light.color = directionalLight[index].color;
	light.attenuation = 1.0;
	return light;
}

float attenuation(float dist, float r) 
{
	return 1.0;
}

float angleAttenuation(int index) {
	vec3 direction = -otherLights[index].dir;
	vec2 angles = otherLights[index].angle;
	return 1.0;
}

Light GetOtherLight(int index, Surface surface) {
	vec3 lightDir = otherLights[index].pos.xzy - surface.pos;
	float atten = attenuation(length(lightDir), otherLights[index].pos.w);
	float attenAngle = angleAttenuation(index);

	Light light;
	light.dir = normalize(lightDir);
	light.color = otherLights[index].color;
	light.attenuation = atten * attenAngle;
	return light;
}

vec3 calculateLight(Surface surface, Light light) {
	vec3 reflectDir = reflect(-light.dir, surface.normal);

	float diff = clamp(dot(surface.normal, light.dir), 0.0, 1.0);
	vec3 diffuse = diff * surface.diffuse;

	float spec = pow(clamp(dot(reflectDir, surface.viewDir), 0.0, 1.0), surface.shininess);
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
