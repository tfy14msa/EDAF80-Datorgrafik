#version 410

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec3 texcoords;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;
//uniform vec3 camera_position;
uniform float ttime;

out VS_OUT {
	vec3 vertex;
	vec3 normal;
	//vec3 world_camera;
	vec3 texcoords;
	vec3 tangent;
	vec3 binormal;
} vs_out;


void main()
{
vec2 amplitude = vec2(1.0, 0.5);
float A1 = amplitude.x;
float A2 = amplitude.y;
vec2 frequency = vec2(0.2, 0.4);
float f1 = frequency.x;
float f2 = frequency.y;
vec2 phase = vec2(0.5, 1.3);
float tp1 = ttime*phase.x;
float tp2 = ttime*phase.y;
vec2 sharpness = vec2(2.0, 2.0);
float k1 = sharpness.x;
float k2 = sharpness.y;

//mat2 direction = mat2(vec2(−1.0, 0.0),vec2(-0.7, 0.7));
//vec2 dir1 = direction.x;
//vec2 dir2 = direction.y;
vec2 dir1 = vec2(-1.0, 0.0);
vec2 dir2 = vec2(-0.7f, 0.7f);
float Dx1 = dir1.x;
float Dx2 = dir2.x;
float Dz1 = dir1.y;
float Dz2 = dir2.y;


float x = vertex.x;
float z = vertex.z;


float angle1 = (Dx1*x+Dz1*z)*f1+tp1;
float angle2 = (Dx2*x+Dz2*z)*f2+tp2;

 float G1 =  A1 * pow((sin(angle1) * 0.5 + 0.5), k1);
 float G2 =  A2 * pow((sin(angle2) * 0.5 + 0.5), k2);

float y = vertex.y + G1 + G2;


float dG1 = k1*A1*pow((sin(angle1) * 0.5 + 0.5), k1-1)*0.5*cos(angle1);
float dG2 = k2*A2*pow((sin(angle2) * 0.5 + 0.5), k2-1)*0.5*cos(angle2);

float dG1dx = dG1*Dx1*f1;
float dG1dz = dG1*Dz1*f1;
float dG2dx = dG2*Dx2*f2;
float dG2dz = dG2*Dz2*f2;

//float big_deriv_x1 = k1*A1*pow(sin(angle1)*0.5+0.5,k1-1);
//float dtemp_1 = 0.5*cos(angle1)*f1;

//float big_deriv_x2 = k1*A1*pow(sin(angle2)*0.5+0.5,k1-1);
//float dtemp_2 = 0.5*cos(angle2)*f2;

//float big_deriv_z1 = k2*A2*pow(sin(angle1)*0.5+0.5,k2-1);

//float big_deriv_z2 = k2*A2*pow(sin(angle2)*0.5+0.5,k2-1);


//float dHdx1 = big_deriv_x1*dtemp_1*Dx1;
//float dHdx2 = big_deriv_x2*dtemp_2*Dx2;
//float dHdz1 = big_deriv_z1*dtemp_1*Dz1;
//float dHdz2 = big_deriv_z2*dtemp_2*Dz2;

//float dHdx = dHdx1 + dHdx2;
//float dHdz = dHdz1 + dHdz2;

float dHdx = dG1dx + dG2dx;
float dHdz = dG1dz + dG2dz;

	vs_out.vertex = vec3(vertex_model_to_world * vec4(x,y,z, 1.0));
	vs_out.normal = vec3(normal_model_to_world * vec4(vec3(-dHdx,1.0,-dHdz), 0.0));
	vs_out.texcoords = texcoords;
	//vs_out.world_camera = camera_position - (vertex_model_to_world * vec4(vertex, 1.0)).xyz;
	
	vs_out.binormal = vec3(normal_model_to_world * vec4(vec3(1.0,dHdx,0.0), 0.0));
	vs_out.tangent = vec3(normal_model_to_world * vec4(vec3(0.0,dHdz,1.0), 0.0));

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}



