#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vs_out {
	vec3 Normal;
	vec3 WSPosition;
	vec2 TexCoords;
} varyings;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 invModel;

void main()
{
	varyings.TexCoords = aUV;
	varyings.Normal = invModel * aNormal;
	varyings.WSPosition = vec3(model * vec4(aPos, 1.0f));
	gl_Position = projection * view * vec4(varyings.WSPosition, 1.0);
}