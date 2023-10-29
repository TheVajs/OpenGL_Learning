#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 iNormal;
out vec3 iFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	iNormal = aNormal;
	iFragPos = vec3(model * vec4(aPos, 1.0f));
	gl_Position = projection * view * vec4(iFragPos, 1.0);
}