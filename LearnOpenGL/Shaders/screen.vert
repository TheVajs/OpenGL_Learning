#version 330 core

out vec2 TexCoords;

void main() {
	vec2 pos;
	pos.x = gl_VertexID % 2 == 1 ? 3.0 : -1.0;
	pos.y = gl_VertexID < 2 ? -1.0 : 3.0;
	gl_Position = vec4(pos, 0.0, 1.0);
	TexCoords = pos;
}
