varying highp vec2 outTextCoord;
varying lowp vec4 outColor;


void main(void)
{
    // helper colors
    vec4 cNone = vec4(0.0,0.0,0.0,0.0);
    
    // derive color
    vec4 fragColor = outColor;
    vec2 pos = mod(outTextCoord.xy, vec2(1.0)) - vec2(0.5);
    float dist = length(pos);
    
    // radii of circle
    const float radii = 0.5;
    fragColor = mix(fragColor, cNone, smoothstep(radii - 0.02, radii, dist));
    
    gl_FragColor = fragColor;
}
