#version 330 core

in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

// http://devernay.free.fr/cours/opengl/materials.html
// adss = ambient, diffuse, specular, shiness (old vay to simulate materials)
// aabb = axis align bounding box
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 pos;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;

uniform vec3 uCameraPos;

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

void main() {
	vec3 ambient = material.ambient * light.ambient;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.pos - FragPos);
	float diff = saturate(dot(normal, lightDir));
	vec3 diffuse = diff * material.diffuse * light.diffuse;

	vec3 view = normalize(uCameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(saturate(dot(reflectDir, view)), material.shininess);
	vec3 specular = spec * material.specular * light.specular;

	vec3 color = ambient + diffuse + specular;
	FragColor = vec4(color, 1.0);
}
