#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main() {
	FragColor = texture2D(screenTexture, TexCoords * .5 + .5);
}