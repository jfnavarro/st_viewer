#version 330

uniform sampler2D vTexture;
in highp vec4 vColor;
in highp float vVisible;
in highp float vSelected;

out highp vec4 fColor;

void main()
{
    if (!bool(vVisible)) {
        discard;
    }
    
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (!bool(vSelected) && dot(circCoord, circCoord) > 1.0) {
        discard;
    }
    
    fColor = vColor;
}
