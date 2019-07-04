#version 330 core

in vec2 UV;
in vec3 Normal;
in vec3 fragmentPos;

out vec3 color;

uniform sampler2D myTextureSampler;


void main(){

	// Probe texture for color


	//hehe
	vec3 norm = normalize(Normal);
	vec3 lightDirs[3];
	lightDirs[0] = normalize(vec3(1, 1, 1));
    lightDirs[1] = normalize(vec3(2, -3, -7));
    lightDirs[2] = normalize(vec3(1, 6, 2));

	float diffuse = 0;
	for(int i = 0; i < 3; i++){

        //sum all lights affecting the surface
        diffuse += 0.8*(max(dot(norm, lightDirs[i]), 0.0));

	}

	if(diffuse <= 0.2) diffuse = 0.2;


	color = diffuse * texture(myTextureSampler, UV).rgb;




}
