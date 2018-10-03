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
} fs_in;

out vec4 frag_color;

void main()
{
	//vec3 L = normalize(light_position - fs_in.vertex);
	vec3 normal = normalize(fs_in.world_normal);
	//frag_color = vec4(1.0) * clamp(dot(normalize(fs_in.normal), L), 0.0, 1.0);

	n = normalize(fs_in.world_normal);
	l = normalize(fs_in.world_light);
	r = normalize(reflect(-l,n));
	diffuse = diffuse*max(0.0,dot(n,l));
	specular = specular*pow(max(0.0,dot(r,v)),shininess);
	frag_color.xyz = ambient + diffuse + specular;
	frag_color.w = 1.0;
}
