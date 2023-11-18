#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aTangent;
// layout(location = 4) in vec3 aBitangent;

out vs_out {
	vec3 WSPosition;
	vec2 TexCoords;
	vec3 Normal;
	mat3 TBN;
} varyings;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 invModel;

void main() {
	// vec3 normal = invModel * aNormal;
	vec3 normal = vec3(model * vec4(aNormal, 0.0));
	vec3 tangent = vec3(model * vec4(aTangent, 0.0));
	normal = normalize(normal);
	tangent = normalize(tangent);
	// Gram-schmidt precess / re-orthogonalize the TBN
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = cross(normal, tangent);
	varyings.TBN = mat3(tangent, bitangent, normal);
	varyings.Normal = invModel * aNormal;
	
	varyings.WSPosition = vec3(model * vec4(aPos, 1.0));
	varyings.TexCoords = aUV;

	gl_Position = projection * view * vec4(varyings.WSPosition, 1.0);
}