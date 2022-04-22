//  nBody Vertex shader
#version 400 core

layout(location = 4) in vec4 translation;
layout(location = 6) in vec4 color;

out vec4 Color;

in vec4 Vertex;

void main()
{
   //  Remember the color
   Color = color;
   //  Translate sphere
   gl_Position = Vertex;// + translation;
}
