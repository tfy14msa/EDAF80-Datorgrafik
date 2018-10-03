#version 410

//uniform vec3 light_position;
uniform samplerCube my_cube_map;

in VS_OUT{
	//vec3 vertex;
	vec3 world_normal;
	//vec3 texcoords;
	//vec3 tangent;
	//vec3 binormal;
} fs_in;

out vec4 frag_color;

void main()
{
	//vec3 L = normalize(light_position - fs_in.vertex);
	vec3 normal = normalize(fs_in.world_normal);
	//frag_color = vec4(1.0) * clamp(dot(normalize(fs_in.normal), L), 0.0, 1.0);
	frag_color = texture(my_cube_map,normal);
}
