#version 410

varying vec2 v_uv;
out vec4 finalColor;                                                                                                                                                                                                                                                            

uniform sampler2D sprite;
uniform vec4 spriteColor;
uniform float alpha_factor;

void main()
{
	finalColor = spriteColor * texture(sprite, v_uv);
	finalColor *= alpha_factor;
}

