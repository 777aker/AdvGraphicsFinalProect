//
//  nBody fragment shader
//
#version 150

uniform sampler2D star;

in  vec4 Color;
in  vec2 Tex2D;
out vec4 FragColor;

void main()
{
   FragColor = texture2D(star,Tex2D) * Color;
}
