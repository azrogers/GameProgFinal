#version 150 

in mat4 modelview; 

in vec3 frag_position, frag_normal; 
in vec2 frag_uv; 

uniform vec4 color; 
uniform sampler2D tx; 

out vec4 fragColor;

float blendOverlay(float base, float blend) {
	return base<0.5?(2.0*base*blend):(1.0-2.0*(1.0-base)*(1.0-blend));
}

vec3 blendOverlay(vec3 base, vec3 blend) {
	return vec3(blendOverlay(base.r,blend.r),blendOverlay(base.g,blend.g),blendOverlay(base.b,blend.b));
}

vec3 blendOverlay(vec3 base, vec3 blend, float opacity) {
	return (blendOverlay(base, blend) * opacity + base * (1.0 - opacity));
}

void main() { 
    fragColor = vec4(blendOverlay(color.rgb, texture(tx, frag_uv).rgb), color.a);
}