varying highp vec4 textCoord;
varying lowp vec4 outColor;
varying lowp float outSelected;
varying lowp float outShape;

// bandpass smooth filter   __/  \__
float smoothband(float lo, float hi, float e, float t) {
    return (lo < hi) ?
    smoothstep(lo-e, lo+e, t) - smoothstep(hi-e, hi+e, t) :
    1.0 - (smoothstep(hi-e, hi+e, t) - smoothstep(lo-e, lo+e, t));
}


void main(void)
{
    // helper colors
    vec4 cNone = vec4(0.0,0.0,0.0,0.0);
    vec4 cWhite = vec4(1.0,1.0,1.0,1.0);
    
    // derive color
    vec4 fragColor = outColor;
    
    // input options
    bool selected = bool(outSelected);
    int shape = int(outShape);
    
    if (shape == 0) { //circle
        // calculate distance from center
        vec2 pos = mod(textCoord.xy, vec2(1.0)) - vec2(0.5);
        float dist = length(pos);
    
        // radii of circle
        float radii = (selected) ? 0.3 : 0.5;
    
        fragColor = mix(fragColor, cNone, smoothstep(radii - 0.02, radii, dist));
        if (selected) {
            fragColor = mix(fragColor, cWhite, smoothband(radii + 0.02, 0.49, 0.01, dist));
        }
    } else if (shape == 1) { //cross
        // calculate distance from center
        vec2 pos = abs(mod(textCoord.xy, vec2(1.0)) - vec2(0.5));
        float mindist = min(pos.x, pos.y);
        float maxdist = max(pos.x, pos.y);
        
        // radii of circle
        float radii = 0.2;
        
        fragColor = mix(fragColor, cNone, smoothstep(radii - 0.02, radii, mindist));
        if (selected) {
            fragColor = mix(fragColor, cWhite, smoothstep(radii + 0.02, radii + 0.04, mindist));
        }
        fragColor = mix(fragColor, cNone, smoothstep(0.5 - 0.02, 0.5, maxdist));
    } else { //rectangle
        // calculate distance from center
        vec2 pos = abs(mod(textCoord.xy, vec2(1.0)) - vec2(0.5));
        float dist = max(pos.x, pos.y);
        
        // radii of circle
        float radii = (selected) ? 0.3 : 0.5;
        
        fragColor = mix(fragColor, cNone, smoothstep(radii - 0.02, radii, dist));
        if (selected) {
            fragColor = mix(fragColor, cWhite, smoothband(radii + 0.02, 0.49, 0.01, dist));
        }
    }
    
    gl_FragColor = fragColor;
}
