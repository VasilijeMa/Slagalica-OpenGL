#version 330 core

in vec2 chCol;
out vec4 outCol;

uniform float red;

void main()
{
	outCol = vec4(chCol.r + red, chCol.g - red, 0.0, 1.0);
}