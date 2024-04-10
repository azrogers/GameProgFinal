#version 150

in vec3 pos;
out vec3 frag_uv;

uniform mat4 projection;
uniform mat4 camera;

void main()
{
	frag_uv = pos;
	gl_Position = projection * camera * vec4(pos, 1.0);
}