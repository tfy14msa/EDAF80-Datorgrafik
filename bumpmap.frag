#version 410

uniform mat4 normal_model_to_world;
uniform sampler2D my_normal_map;
uniform sampler2D my_diffuse;

uniform vec3 ambient; 
uniform vec3 diffuse; 
uniform vec3 specular;
uniform float shininess;


in VS_OUT{
	vec3 world_normal;
	vec3 world_light;
	vec3 world_binormal;
	vec3 world_tangent;
	vec3 world_texcoords;
} fs_in;

out vec4 frag_color;

void main()
{
	//vec3 L = normalize(light_position - fs_in.vertex);
	vec3 normal = normalize(fs_in.world_normal);
	//frag_color = vec4(1.0) * clamp(dot(normalize(fs_in.normal), L), 0.0, 1.0);

	n = normalize(fs_in.world_normal);
	t = normalize(fs_in.world_tangent);
	b = normalize(fs_in.world_binormal);
	l = normalize(fs_in.world_light);
	tex = normalize(fs_in.world_texcoords);
	auto TBN = mat3(T,B,N);

	//vec3 bump = (normalize(normal_model_to_world * (mat4(vec4(T,0),vec4(B,0),
	//			vec4(N,0),	 vec4(0,0,0,1))) * (vec4(ntext, 0)))).xyz;
	//diffuse = diffuse*max(0.0,dot(n,l));
	//specular = specular*pow(max(0.0,dot(r,v)),shininess);
	//frag_color.xyz = ambient + diffuse + specular;
	//frag_color.w = 1.0;
}
