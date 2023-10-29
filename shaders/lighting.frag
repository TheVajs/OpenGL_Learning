#version 330 core

in vec3 iNormal;
in vec3 iFragPos;
out vec4 FragColor;

uniform vec3 uObjectColor;
uniform vec3 uAmbient;
uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uCameraPos;

void main() 
{
	vec3 normal = normalize(iNormal);
	vec3 lightDir = normalize(uLightPos - iFragPos);
	vec3 diffuse = max(0, dot(normal, lightDir)) * uLightColor;

	float specStrength = .6f;
	vec3 view = normalize(uCameraPos - iFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(0, dot(reflectDir, view)), 32);
	vec3 specular = specStrength * spec * uLightColor;

	vec3 color = (uAmbient + diffuse + specular) * uObjectColor;

	FragColor = vec4(color, 1.0f);
}
