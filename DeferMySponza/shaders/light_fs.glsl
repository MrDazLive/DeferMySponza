#version 400

#define PI 3.1415926535897932384626433832795

struct Material {
	vec3 diffuse;
	float shine;
	vec3 specular;
	float metallic;

	float rough;
	int mainTexture;
	int normalTexture;
	int excess2;
};

layout(std140) uniform block_material{
	Material material[7];
};

struct Light {
	vec3 position;
	vec3 direction;
	vec3 intensity;
	float range;
	float coneAngle;
};

uniform vec3 eyePosition;
uniform vec3 eyeDirection;

uniform sampler2DRect colourMap;
uniform sampler2DRect positionMap;
uniform sampler2DRect normalMap;
uniform sampler2DRect materialMap;

uniform	sampler2DRect shadowMap[5];

subroutine vec3 LightType(Light l);
subroutine uniform LightType lightSelection;

flat in int fixed_instance;
flat in	mat4 fixed_projection;
flat in Light fixed_light;

layout(location = 0)out vec3 fragment_colour;

vec3 Colour;
vec3 WorldPosition;
vec3 Normal;
vec2 TextureCoordinate;
int MaterialID;

const int attenuation = 10;
const int shadowScale = 256;
const float shadowBias = -0.001f;

void getShine(vec3 lightDirection, inout float specular) {
	vec3 H = normalize(lightDirection + eyeDirection);
	float VH = max(dot(eyeDirection, H), 0);

	float shine = pow(1 - VH, 5);
	shine *= (1 - material[MaterialID].shine);
	specular *= shine + material[MaterialID].shine;
}

void getMetallic(inout float diffuse, inout float specular) {
	float met = material[MaterialID].metallic;
	specular = mix(specular, 1, met);
	diffuse *= (1 - met);
}

/*vec3 getInternal(vec3 lightDirection, vec3 lightIntensity) {
	float diffuse = clamp(dot(Normal, lightDirection), 0, 1);
	if (diffuse > 0) {
		vec3 dir = normalize(eyePosition - WorldPosition);
		vec3 ref = normalize(reflect(lightDirection, Normal));

		float specular = max(dot(dir, ref), 0);

		getShine(lightDirection, specular);
		getMetallic(diffuse, specular);

		return vec3(diffuse + specular) * Colour * lightIntensity;
	}
	return vec3(0);
}*/

float getDiffuse(vec3 lightDirection) {
	vec3 L = normalize(lightDirection);
	vec3 V = normalize(eyeDirection);
	vec3 H = normalize(lightDirection + eyeDirection);

	float cosL = max(dot(L, Normal), 0);
	float cosV = max(dot(V, Normal), 0);
	float cosH = max(dot(H, Normal), 0);
	float cosD = max(dot(L, H), 0);
	
	float FL = pow((1- cosL), 5);
	float FV = pow((1- cosV), 5);
	float R = 2 * material[MaterialID].rough * cosD * cosD;

	float RetroReflect = R * (FL + FV + (FL * FV * (R - 1)));
	float Diffuse = (1 - 0.5f * FL) * (1 - 0.5f * FV) + RetroReflect;
	Diffuse /= PI;

	return Diffuse;
}

float getSpecular(vec3 lightDirection) {
	vec3 L = normalize(lightDirection);
	vec3 V = normalize(eyeDirection);
	vec3 H = normalize(lightDirection + eyeDirection);
	
	float cosL = max(dot(L, Normal), 0);
	float cosV = max(dot(V, Normal), 0);
	float cosH = max(dot(H, Normal), 0);
	float cosE = max(dot(V, H), 0);

	float F = pow((1 - cosE), 5);
	float G = min(1, min(2*cosH*cosV/cosE, 2*cosH*cosL/cosE));
	float D = 1.0f;//exp((cosH * cosH - 1) / (material[MaterialID].rough * material[MaterialID].rough * cosH * cosH)) / (4 * material[MaterialID].rough * material[MaterialID].rough * cosH * cosH);

	float B = 1.0f;//PI * cosL * cosV;

	float Specular = F * G * D / B;

	return Specular;
}

vec3 getInternal(vec3 lightDirection, vec3 lightIntensity) {
	if(dot(Normal, lightDirection) > 0) {
		float D = getDiffuse(lightDirection);
		float S = getSpecular(lightDirection);
		
		getShine(lightDirection, S);
		getMetallic(D, S);

		return (D + S) * Colour * lightIntensity;
	}
	return vec3(0);
}

subroutine(LightType) vec3 Directional(Light l) {
	return getInternal(l.direction, l.intensity);
}

subroutine(LightType) vec3 Point(Light l) {
	vec3 dir = l.position - WorldPosition;
	float dis = length(dir);
	dir = normalize(dir);

	float att = l.range * l.range / (dis * dis * attenuation);

	return att * getInternal(dir, l.intensity);
}

subroutine(LightType) vec3 Spot(Light l) {
	vec3 dir = l.position - WorldPosition;
	float dis = length(dir);
	dir = normalize(dir);

	float fac = dot(l.direction, -dir);
	float ang = cos(l.coneAngle / 2);

	if (fac > ang) {
		float dAtt = l.range * l.range / (dis * dis * attenuation);
		float cAtt = 1 - ((1 - fac) / (1 - ang));

		vec4 ShadowPos = fixed_projection * vec4(WorldPosition, 1);
		vec3 Shadow = ShadowPos.xyz / ShadowPos.w;
		float vis = (texture(shadowMap[fixed_instance], Shadow.xy * shadowScale).r < Shadow.z + shadowBias) ? 0.0f : 1.0f;

		return dAtt * cAtt * getInternal(dir, l.intensity) * vis;
	}
	return vec3(0);
}

void main(void) {
	Colour = texture(colourMap, gl_FragCoord.xy).xyz;
	WorldPosition = texture(positionMap, gl_FragCoord.xy).xyz;
	Normal = texture(normalMap, gl_FragCoord.xy).xyz;
	TextureCoordinate = texture(materialMap, gl_FragCoord.xy).xy;
	MaterialID = int(texture(materialMap, gl_FragCoord.xy).z);

	fragment_colour = lightSelection(fixed_light);
}