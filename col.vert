#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inCol;
out vec2 chCol;
uniform float kY;
uniform float minY;

void main()
{
	chCol = inCol;
	gl_Position = vec4(inPos.x, max(inPos.y * kY, minY), 0.0, 1.0);
	
}