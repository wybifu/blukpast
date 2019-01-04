#version 410

//layout (location = 0) in vec4 vertices;
in vec4 vertices;  // vec2 pos vec2 tex
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(vertices.xy, 0.0, 1.0);
	TexCoords = vertices.zw;
}
