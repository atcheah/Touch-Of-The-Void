#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;

vec4 color_shift(vec4 in_color) 
{
	return in_color;
}

vec4 fade_color(vec4 in_color) 
{
	if (darken_screen_factor > 0)
		//in_color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);
		in_color *= (1 - darken_screen_factor);
	return in_color;
}

void main()
{
    vec4 in_color = texture(screen_texture, texcoord);
    color = color_shift(in_color);
    color = fade_color(color);
}