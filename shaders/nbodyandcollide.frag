//
//  nBody fragment shader
//
#version 150

in vec4 Color;
in vec2 Tex2D;
out vec4 FragColor;

void main()
{
	// so this frag shader is really complicated
	// basically, I take in the color passed by the vert shader
	// and do absolutely nothing to it but pass it even further along
	FragColor = Color;
}
