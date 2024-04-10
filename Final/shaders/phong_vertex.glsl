#version 150

uniform mat4 transform, camera, projection;
uniform vec3 cameraPos;

in vec3 position, normal;
in vec2 uv;

out vec3 frag_position, frag_normal, frag_position_camera;
out vec2 frag_uv; 
out vec3 view_dir;

out mat4 view;

void main() {
    mat4 modelview = camera * transform;
	vec4 eye_position = modelview * vec4(position, 1.0);
    gl_Position = projection * eye_position;
    frag_position_camera = gl_Position.xyz;
	frag_position = vec3(transform * vec4(position, 1.0));
	frag_uv = uv;
	if(normal.x == 0 && normal.y == 0 && normal.z == 0)
		frag_normal = vec3(0, 0, 0);
	else 
		frag_normal = normalize(transpose(inverse(mat3(transform))) * normal);
	view_dir = normalize(cameraPos - frag_position);

	view = camera;
}