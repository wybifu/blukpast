#version 410

in vec2 vertices;

uniform mat4 model;
uniform mat4 view ;
uniform mat4 projection;

void main()
{
	gl_Position = view * projection * model * vec4(vertices.xy, 0.0f, 1.0f);
}
