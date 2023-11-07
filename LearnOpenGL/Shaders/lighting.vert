#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_model;

void main()
{
	TexCoords = aUV;
	Normal = normal_model * aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0f));
	gl_Position = projection * view * vec4(FragPos, 1.0);
}