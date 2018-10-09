#version 410

layout (location = 0) in vec3 vertex;
//layout (location = 2) in vec3 texcoords;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

uniform float ttime;

out VS_OUT {
	vec3 vertex;
	vec3 normal;
} vs_out;


void main()
{
vec2 amplitude = vec2(1.0, 0.5);
float Ax = amplitude.x;
float Az = amplitude.y;
vec2 frequency = vec2(0.2, 0.4);
float fx = frequency.x;
float fz = frequency.y;
vec2 phase = vec2(0.5, 1.3);
float tpx = ttime*phase.x;
float tpz = ttime*phase.y;
vec2 sharpness = vec2(2.0, 2.0);
float kx = sharpness.x;
float kz = sharpness.y;

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

float big_deriv_x1 = kx*Ax*pow(sin((Dx1*x+Dz1*z)*fx+tpx)*0.5+0.5,kx-1);
float dtemp_1 = 0.5*cos((Dx1*x+Dz1*z)*fx+tpx)*fx;

float big_deriv_x2 = kx*Ax*pow(sin((Dx2*x+Dz2*z)*fx+tpx)*0.5+0.5,kx-1);
float dtemp_2 = 0.5*cos((Dx2*x+Dz2*z)*fx+tpx)*fx;

float big_deriv_z1 = kz*Az*pow(sin((Dx1*x+Dz1*z)*fz+tpz)*0.5+0.5,kz-1);


float big_deriv_z2 = kz*Az*pow(sin((Dx2*x+Dz2*z)*fz+tpz)*0.5+0.5,kz-1);


float dHdx1 = big_deriv_x1*dtemp_1*Dx1;
float dHdx2 = big_deriv_x2*dtemp_2*Dx2;
float dHdz1 = big_deriv_z1*dtemp_1*Dz1;
float dHdz2 = big_deriv_z2*dtemp_2*Dz2;

float dHdx = dHdx1 + dHdx2;
float dHdz = dHdz1 + dHdz2;


	vs_out.vertex = vec3(vertex_model_to_world * vec4(vertex, 1.0));
	vs_out.normal = vec3(normal_model_to_world * vec4(vec3(-dHdx,1.0,-dHdz), 0.0));

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}



