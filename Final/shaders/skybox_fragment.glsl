#version 150

in vec3 frag_uv;
out vec4 fragColor;

uniform vec3 fogColor;
uniform float fogDensity;

uniform samplerCube skybox;

void main()
{
	fragColor = mix(vec4(fogColor, 1.0), texture(skybox, frag_uv), frag_uv.y);
}