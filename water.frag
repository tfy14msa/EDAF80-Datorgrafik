#version 410

uniform vec3 light_position;

in VS_OUT {
	vec3 vertex;
	vec3 normal;
} fs_in;

out vec4 frag_color;

void main()
{
vec3 N = normalize(fs_in.normal);
vec3 V = normalize(fs_in.vertex);

vec4 color_deep = vec4(0.0,0.0,0.1,1.0);
vec4 color_shallow = vec4(0.0,0.5,0.5,1.0);




float facing = 1 - max(dot(V,N),0.0);
vec4 color_water = mix(color_deep,color_shallow,facing);
//n = 1.33; // air -> water
//n = 1.0/1.33; //water -> air

//R0 = pow((n1-n2)/(n1+n2),2);
//R0 = 0.02037 // air -> water
//fresnel = R0 + (1-R0)*pow(1-dot(V,N),5);

//vec4 refraction  = refract(I, N, n);
	vec3 L = normalize(light_position - fs_in.vertex);
	frag_color = color_water ;//+ reflection*fresnel + refraction*(1-fresnel);
}
