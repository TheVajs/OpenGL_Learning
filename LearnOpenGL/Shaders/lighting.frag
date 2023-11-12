#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
out vec4 FragColor;

// http://devernay.free.fr/cours/opengl/materials.html
// adss = ambient, diffuse, specular, shiness (old vay to simulate materials)
// aabb = axis align bounding box
struct Material {
	bool light_maps;
	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Surface {
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

Surface GetSurface(Material material) {
	Surface surface;
	if (material.light_maps) {
		surface.diffuse = vec3(texture(material.texture_diffuse0, TexCoords));
		surface.specular = vec3(texture(material.texture_specular0, TexCoords));
	} else {
		surface.diffuse = material.diffuse;
		surface.specular = material.specular;
	}
	surface.shininess = material.shininess;
	return surface;
}

#define ATTENUATION_QUADRATIC
struct Light {
	vec3 pos;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

#ifdef ATTENUATION_QUADRATIC
	float constant;
	float linear;
	float quadratic;
#endif
};

float attenuation(Light light, float d) {
#ifdef ATTENUATION_QUADRATIC
	return 1.0 / (light.constant + light.linear * d + light.quadratic * d * d);
#else
	return 1.0;
#endif
}

// uniform float uTime;
uniform Material material;
uniform Light light;

uniform vec3 uCameraPos;

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

void main() {
	Surface surface = GetSurface(material);

	vec3 ambient = surface.diffuse * light.ambient;

	vec3 normal = normalize(Normal);
	vec3 posLight = light.pos - FragPos;
	vec3 lightDir = normalize(posLight);
	float diff = saturate(dot(normal, lightDir));
	vec3 diffuse = diff * surface.diffuse * light.diffuse;

	vec3 viewDir = normalize(uCameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(saturate(dot(reflectDir, viewDir)), surface.shininess);
	vec3 specular = spec * surface.specular * light.specular;

	float dist = posLight.length();
	float atten = attenuation(light, dist);
	ambient *= atten;
	diffuse *= atten;
	specular *= atten;

	vec3 color = ambient + diffuse + specular;
	FragColor = vec4(color, 1.0); // vec4(normal.xyz * 0.5f + 1.0f, 1.0f); // 
}