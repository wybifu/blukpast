#version 410

in vec4 vertices;

varying vec2 v_uv;

uniform mat4 view;                                                                                                                                                                                                                                                             
uniform mat4 model;                                                                                                                                                                                                                                                             
uniform mat4 projection;
uniform vec4 vt_;

void main()
{
	v_uv.x = (vertices.z * vt_[1]) + vt_[0];
	v_uv.y = (vertices.w * vt_[3]) + vt_[2];

    gl_Position = view * projection  * model * vec4(vertices.xy, 0.0f, 1.0f);
}

