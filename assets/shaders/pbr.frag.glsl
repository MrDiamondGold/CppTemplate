#version 450

#define PI 3.141

in vec2 f_uv;
in vec3 f_normal;
in vec3 f_tangent;
in vec3 f_frag_pos;

out vec4 o_frag;

uniform vec4 u_albedo;
uniform sampler2D s_albedo;

uniform float u_normal;
uniform sampler2D s_normal;

uniform float u_metallic;
uniform sampler2D s_metallic;

uniform float u_roughness;
uniform sampler2D s_roughness;

uniform float u_ao;
uniform sampler2D s_ao;

// uniform samplerCube s_cubemap;
// uniform samplerCube s_irradiance;

struct SunLight {
	vec3 direction;
	vec3 color;
};

uniform SunLight u_light;
uniform vec3 u_view_pos;

vec3 get_normal()
{
    vec3 tangent_normal = (texture(s_normal, f_uv).xyz) * 2.0 - 1.0;

    vec3 Q1 = dFdx(f_frag_pos);
    vec3 Q2 = dFdy(f_frag_pos);
    vec2 st1 = dFdx(f_uv);
    vec2 st2 = dFdy(f_uv);

    vec3 N = normalize(f_normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return mix(N, normalize(TBN * tangent_normal), u_normal);
}

// Distribution
float distribution_ggx(vec3 n, vec3 h, float roughness) {
	float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(n, h), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom / denom;
}

// Geometry sub
float schlick_ggx(float NdotV, float roughness) {
    float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float demon = NdotV * (1.0 - k) + k;

	return nom / demon;
}

// Geometry
float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness) {
    float NdotV = max(dot(n, v), 0.0);
    float NdotL = max(dot(n, l), 0.0);
    float ggx2 = schlick_ggx(NdotV, roughness);
    float ggx1 = schlick_ggx(NdotL, roughness);
	
    return ggx1 * ggx2;
}

// Fresnel Reflectance
// Of the light hitting a mirror what percentage is reflected?
vec3 schlick_fresnel_reflectance(float cosTheta, vec3 F0, float roughness) {
	return min(F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0), 1.0);
}

void main() {
	float albedo_a = texture(s_albedo, f_uv).a;
    vec3 albedo = pow(texture(s_albedo, f_uv).rgb * u_albedo.rgb, vec3(2.2));
	float metallic = texture(s_metallic, f_uv).b * u_metallic;
	float roughness = texture(s_roughness, f_uv).g * u_roughness;
	float ao = texture(s_ao, f_uv).r * u_ao;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// vec3 n = normalize(f_normal);
	vec3 n = get_normal();
	vec3 v = normalize(u_view_pos - f_frag_pos); // View direction
	vec3 r = reflect(-v, n);

	vec3 o = vec3(0.0);

    vec3 l = normalize(-u_light.direction); // Light direction
    vec3 h = normalize(l + v); // Halfway vector
    // float dist = length(u_light.position - f_frag_pos) / u_lights[i].size;
    float attenuation = 1.0;
    vec3 radiance = u_light.color * attenuation;

    vec3 F = schlick_fresnel_reflectance(max(dot(h, v), 0.0), F0, roughness);
    float G = geometry_smith(n, v, l, roughness);
    float D = distribution_ggx(n, h, roughness);
    
    vec3 kS = F; // Specular
    vec3 kD = vec3(1.0) - kS; // Diffuse
    kD *= 1.0 - metallic;

    vec3 numerator = D * G * F;
    float denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);
    
    float NdotL = max(dot(n, l), 0.0);
    o += (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.03 * albedo * ao);
	vec3 color = ambient + o;

	color = color / (color + vec3(1.0)); // Gamma 
	color = pow(color, vec3(1.0 / 2.2)); // correction

    vec3 tangent_normal = texture(s_normal, f_uv).xyz;

	o_frag = vec4(color, albedo_a);
    // o_frag = vec4(tangent_normal, 1.0);
}