#version 150 

/* Custom Light struct */
#define LIGHT_POINT 0
#define LIGHT_SPOT 1
#define LIGHT_DIR 2
struct Light
{
	// Light type: 0 = point, 1 = spotlight, 2 = directional
	int type;

	vec3 position;
	vec3 color;
	vec3 ambient;
	float falloff;

	float radius;

	// Spotlights
	vec3 direction;
	float innerAngle;
	float spotAngle;
};

in mat4 view;

in vec3 frag_position, frag_normal, frag_position_camera;
in vec2 frag_uv; 
in vec3 view_dir;

uniform vec4 color; 
uniform sampler2D txDiffuse;
uniform sampler2D txNormal;
uniform sampler2D txSpecular;
uniform sampler2D txAlpha;
uniform bool txAvailable[4] = {false, false, false, false};
uniform float specularScale = 0.15f;
uniform float shininess = 5.0f;
uniform float roughness = 1.0f;
uniform float albedo = 0.95f;
uniform float time;
uniform bool wireframe = false;
uniform bool normalOnly = false;
uniform float fogDensity = 0;
uniform vec3 fogColor = vec3(0.0, 0.0, 0.0);

#define MAX_LIGHTS 16
uniform Light lights[MAX_LIGHTS];
uniform int numLights = 0;

out vec4 fragColor;
out vec3 lightPos;

const float gamma = 2.2;

/* Gamma correction */

float toLinear(float v) {
	return pow(v, gamma);
}

vec2 toLinear(vec2 v) {
	return pow(v, vec2(gamma));
}

vec3 toLinear(vec3 v) {
	return pow(v, vec3(gamma));
}

vec4 toLinear(vec4 v) {
	return vec4(toLinear(v.rgb), v.a);
}

vec4 textureLinear(sampler2D uTex, vec2 uv) 
{
	return toLinear(texture2D(uTex, uv));
}

float toGamma(float v) {
  return pow(v, 1.0 / gamma);
}

vec2 toGamma(vec2 v) {
  return pow(v, vec2(1.0 / gamma));
}

vec3 toGamma(vec3 v) {
  return pow(v, vec3(1.0 / gamma));
}

vec4 toGamma(vec4 v) {
  return vec4(toGamma(v.rgb), v.a);
}

/* Normal maps */
mat3 cotangentFrame(vec3 N, vec3 p, vec2 uv) {
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx(p);
	vec3 dp2 = dFdy(p);
	vec2 duv1 = dFdx(uv);
	vec2 duv2 = dFdy(uv);

	// solve the linear system
	vec3 dp2perp = cross(dp2, N);
	vec3 dp1perp = cross(N, dp1);
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	// construct a scale-invariant frame 
	float invmax = 1.0 / sqrt(max(dot(T,T), dot(B,B)));
	return mat3(T * invmax, B * invmax, N);
}

vec3 perturb(vec3 map, vec3 N, vec3 V, vec2 texcoord) {
	mat3 TBN = cotangentFrame(N, V, texcoord);
	return normalize(TBN * map);
}

/* Light attenuation */
float attenuation(float r, float f, float d)
{
	float denom = d / r + 1.0;
	float attenuation = 1.0 / (denom*denom);
	float t = (attenuation - f) / (1.0 - f);
	return max(t, 0.0);
}

float simpleAttenuation(float att, float distance)
{
	return 1.0 / (1.0 + att * pow(distance, 2));
}

/* Oren-Nayar Diffuse Shading */
float orenNayarDiffuse(
	vec3 lightDirection,
	vec3 viewDirection,
	vec3 surfaceNormal,
	float roughness,
	float albedo) {

	float LdotV = dot(lightDirection, viewDirection);
	float NdotL = dot(lightDirection, surfaceNormal);
	float NdotV = dot(surfaceNormal, viewDirection);

	float s = LdotV - NdotL * NdotV;
	float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

	float sigma2 = roughness * roughness;
	float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
	float B = 0.45 * sigma2 / (sigma2 + 0.09);

	return albedo * max(0.0, NdotL) * (A + B * s / t) / 3.14159;
}

/* Specular */
float blinnPhongSpecular(
  vec3 lightDirection,
  vec3 viewDirection,
  vec3 surfaceNormal,
  float shininess) 
{
	vec3 H = normalize(viewDirection + lightDirection);
	return pow(max(0.0, dot(surfaceNormal, H)), shininess);
}

float phongSpecular(
  vec3 lightDirection,
  vec3 viewDirection,
  vec3 surfaceNormal,
  float shininess) {

  //Calculate Phong power
  vec3 R = -reflect(lightDirection, surfaceNormal);
  return pow(max(0.0, dot(viewDirection, R)), shininess);
}

float getSpecular(vec2 uv)
{
	if(txAvailable[2])
		return textureLinear(txSpecular, uv).r;
	return 1.0f;
}

vec3 calcLight(Light light, vec3 V, vec3 N, vec3 fragPos, vec2 uv, vec3 viewDir)
{
	vec3 lightVector = light.position - fragPos;
	float lightDistance = length(lightVector);
	vec3 L = normalize(lightVector);

	float specScale = specularScale;
	float falloff = 0.0f;
	if(light.type == LIGHT_DIR)
	{
		specScale = 0;
		falloff = 1.0f;
		L = normalize(-light.direction);
	}
	else if(light.type == LIGHT_POINT)
	{
		falloff = attenuation(light.radius, light.falloff, lightDistance);
	}
	else if(light.type == LIGHT_SPOT)
	{
		float theta = dot(L, normalize(-light.direction));
		float epsilon = light.innerAngle - light.spotAngle;
		falloff = toLinear(clamp((theta - light.spotAngle) / epsilon, 0.0, 1.0));
	}

	vec3 diffuse = light.color * orenNayarDiffuse(L, V, N, roughness, albedo);
	vec3 diffuseColor = color.xyz;
	if(txAvailable[0])
		diffuseColor = textureLinear(txDiffuse, uv).rgb;
	diffuse = diffuse * diffuseColor;

	vec3 ambient = light.ambient * diffuseColor;

	float specularStrength = getSpecular(uv);
	float specular = specularStrength * blinnPhongSpecular(L, viewDir, N, shininess);
	specular *= specScale;
	specular *= falloff;
	diffuse *= falloff;
	ambient *= falloff;

	return diffuse + ambient + specular;
}

/* Fog calculation */
float fogExp(float distance, float density)
{
	return 1.0 - clamp(exp(-density * distance), 0.0, 1.0);
}

void main() 
{
	if(wireframe)
	{
		fragColor = color;
		return;
	}
	else if(numLights == 0) // no lights, fullbright
	{
		if(txAvailable[0])
			fragColor = texture2D(txDiffuse, frag_uv);
		else
			fragColor = color;
		return;
	}

	vec3 V = normalize(frag_position);

	float alpha = 1.0f;
	if(txAvailable[3])
		alpha = 1.0f - texture2D(txAlpha, frag_uv).r;

	if(alpha == 0.0)
		discard;

	float fogDistance = gl_FragCoord.z / gl_FragCoord.w;
	float fogAmount = fogExp(fogDistance, fogDensity);
	if(fogAmount >= 1.0)
		discard;

	vec3 N = frag_normal;
	if(txAvailable[1])
	{
		vec3 normalMap = textureLinear(txNormal, frag_uv).rgb * 2.0 - 1.0;
		normalMap.y *= -1.0;

		N = perturb(normalMap, normalize(frag_normal), normalize(frag_position_camera), frag_uv);
	}

	if(normalOnly)
	{
		fragColor = vec4((N + 1.0) / 2, 1.0);
		return;
	}

	vec3 finalColor = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < numLights; i++)
	{
		finalColor += calcLight(lights[i], V, N, frag_position, frag_uv, view_dir);
	}

	fragColor = vec4(mix(toGamma(finalColor), fogColor, fogAmount), alpha);
}