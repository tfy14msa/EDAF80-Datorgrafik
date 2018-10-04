#version 410

uniform vec3 light_position;
uniform vec3 camera_position;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;


in VS_OUT{
	vec3 world_camera;
	vec3 world_normal;
	vec3 world_light;
} vs_in;

out vec4 frag_color;

void main()
{
	//vec3 L = normalize(light_position - vs_in.vertex);
	
	vec3 n = normalize(vs_in.world_normal);
	vec3 l = normalize(vs_in.world_light);
	vec3 r = normalize(reflect(-l,n));
	auto diffuse = diffuse*max(0.0,dot(n,l));
	auto specular = specular*pow(max(0.0,dot(r,v)),shininess);
	frag_color.xyz = ambient + diffuse + specular;
	frag_color.w = 1.0;
}
