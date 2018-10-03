#version 410

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 texcoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 binormal;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

//int has_textures = 0; //set to 1 if has texture
//int has_diffuse_texture = 0; // set to 1 if attached a diffuse texture to node object
//int has_opacity_texture = 0; // set to 1 if attached a opacity texture to node object 





// This is the custom output of this shader. If you want to retrieve this data
// from another shader further down the pipeline, you need to declare the exact
// same structure as in (for input), with matching name for the structure
// members and matching structure type. Have a look at
// shaders/EDAF80/diffuse.frag.
out VS_OUT{

	vec3 world_normal;
	vec3 world_light;
	vec3 world_binormal;
	vec3 world_tangent;
	vec3 world_texcoords;
	} vs_out;


void main()
{
	vec4 world_normal_4 = (normal_model_to_world * vec4(normal, 0.0));
	vs_out.world_normal = world_normal_4.xyz;
	vec4 world_binormal_4 = (normal_model_to_world * vec4(binormal, 0.0));
	vs_out.world_binormal = world_binormal_4.xyz
	vec4 world_tangent_4 = (normal_model_to_world * vec4(tangent 0.0));
	vs_out.world_tangent = world_tangent_4.xyz	

	vs_out.texcoords = texcoords;
	vs_out.world_light = light_position - (vertex_model_to_world * vec4(vertex, 1.0)).xyz;
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}



