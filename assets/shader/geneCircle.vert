#version 120
// texture coordinate [0.0, 1.0]
attribute vec4 in_texture;
attribute float in_options;
// passed along to fragment shader
varying vec4 out_color;
varying vec4 out_texture;
varying vec2 out_options;
void main(void)
{
	int flags = int(in_options);
	bool selected = mod(flags, 2) >= 1;
    bool complex  = mod(flags, 4) >= 2;

    out_color = gl_Color;
    out_texture = in_texture;
	out_options = vec2(
		(selected) ? 1.0 : 0.0,
		(complex) ? 1.0 : 0.0);
    gl_Position = ftransform();
}
