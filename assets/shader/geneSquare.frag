varying vec4 out_color;
varying vec4 out_texture;
varying vec2 out_options;
// bandpass smooth filter   __/  \__
float smoothband(float lo, float hi, float e, float t) {
    return (lo < hi) ?
                smoothstep(lo-e, lo+e, t) - smoothstep(hi-e, hi+e, t) :
                1.0 - (smoothstep(hi-e, hi+e, t) - smoothstep(lo-e, lo+e, t));
}
void main(void)
{
    // colors
    const vec4 cNone  = vec4(0.0,0.0,0.0,0.0);
    const vec4 cWhite = vec4(1.0,1.0,1.0,1.0);
	// input
	bool selected = (out_options.x > 0.5);
	bool complex  = (out_options.y > 0.5);

    // calculate distance from center
    vec2 pos = abs(mod(out_texture.xy, vec2(1.0)) - vec2(0.5));
    float dist = max(pos.x, pos.y);

    // radii of circle
    float radii = (selected) ? 0.3 : 0.5;

    // derive circle color
    vec4 fragColor = out_color;
         fragColor = mix(fragColor, cNone, smoothstep(radii-0.02, radii, dist));
    if (selected) { fragColor = mix(fragColor, cWhite, smoothband(radii+0.02, 0.49, 0.01, dist)); }
    gl_FragColor = fragColor;
}
