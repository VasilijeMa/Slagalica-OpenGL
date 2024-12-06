#version 330 core 

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;

uniform float uX;
uniform float uY;
uniform float kX;

void main()
{
	if (kX == 0) {
		gl_Position = vec4(inPos.x + uX, inPos.y + uY, 0.0, 1.0);
		chTex = vec2(inTex.s, inTex.t);
	}
	else {
		gl_Position = vec4(inPos.x / kX + uX, inPos.y + uY, 0.0, 1.0);
		chTex = vec2(inTex.s, inTex.t);
	}
}