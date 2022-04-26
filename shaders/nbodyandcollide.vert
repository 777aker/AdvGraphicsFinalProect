//  nBody Vertex shader
#version 400 core
// these are the coordinate arrays for all the instanced icos
in vec4 translation1;
in vec4 translation2;
// and their colors
in vec4 color;
// color to frag shader
out vec4 Color;
// this is the icosahedron
in vec4 Vertex;
// glsl 4 so gotta do this ourselves
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
// which translation array to use
uniform int buf;

vec3 lighting() {
	// ambient (multiply by color)
	vec3 light = color.xyz;
	// light intensity (fake it bc don't need good lighting, need fast lighting)
	light += vec3(.6, .6, .6) * clamp(dot(vec4(3.0, 3.0, 3.0, 1.0), Vertex), .1, 10) * color.xyz;
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
