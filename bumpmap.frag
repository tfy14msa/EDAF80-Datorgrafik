#version 410

uniform mat4 normal_model_to_world;
uniform sampler2D my_bump_map;
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

	vec3 n = normalize(fs_in.world_normal);
	vec3 t = normalize(fs_in.world_tangent);
	vec3 b = normalize(fs_in.world_binormal);
	vec3 l = normalize(fs_in.world_light);
	vec3 tex = fs_in.world_texcoords;
	//mat3 TBN = mat3(t,b,n);

	vec3 ntext = (texture(my_bump_map, tex.xy).xyz)*2-1;
	vec3 bump = normalize(mat3(t,b,n) * ntext);
	
	vec3 diffuse_tex = texture(my_diffuse, tex.xy).xyz;
	
	frag_color.xyz = diffuse_tex*max(dot(l, bump), 0.0);
	//diffuse = diffuse*max(0.0,dot(n,l));
	//specular = specular*pow(max(0.0,dot(r,v)),shininess);
	
	frag_color.w = 1.0;
}
