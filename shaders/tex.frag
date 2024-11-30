#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D background;
uniform sampler2D foreground;
uniform sampler2D lens;

void main()
{   
    vec4 tex0 = texture(background, chTex);
    vec4 tex1 = texture(foreground, chTex);
    vec4 tex2 = texture(lens, chTex);

    if(tex2 == vec4(0.0, 0.0, 0.0, 1.0)) {
        if (tex1 == vec4(0.0, 0.0, 0.0, 1.0) || tex1.a == 0.0)  outCol = tex0;
        else outCol = tex1;
    } else {
        if (tex1 == vec4(0.0, 0.0, 0.0, 1.0) || tex1.a == 0.0) outCol = mix(tex0, tex2, 0.4);
        else outCol = mix(tex1, tex2, 0.4);
    }
}