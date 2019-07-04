#version 330 core

in vec2 UV;
in vec3 Normal;

out vec3 color;

uniform sampler2D myTextureSampler;
uniform vec3 lightLocationsX[25];
uniform vec3 lightLocationsY[25];
uniform vec3 lightLocationsZ[25];

void main(){

	// Output color = color of the texture at the specified UV
	color = texture( myTextureSampler, UV ).rgb;
}
