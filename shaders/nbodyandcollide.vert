//  nBody Vertex shader
#version 400 core

in vec4 translation1;
in vec4 translation2;
in vec4 color;

out vec4 Color;

in vec4 Vertex;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

uniform int buf;

vec3 lighting() {
	// ambient (multiply by color)
	vec3 light = color.xyz;
	// light intensity
	light += vec3(.6, .6, .6) * dot(vec4(3.0, 3.0, 3.0, 1.0), Vertex) * color.xyz;
	// return 
	return light;
}

void main()
{
	//  Remember the color
	Color = vec4(lighting(), 1);
	//  Translate sphere
	if(buf == 0) {
		gl_Position = ProjectionMatrix * ModelViewMatrix * (Vertex + translation1);
	} else {
		gl_Position = ProjectionMatrix * ModelViewMatrix * (Vertex + translation2);
	}
}
