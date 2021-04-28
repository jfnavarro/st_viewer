#version 330

flat in highp vec4 vColor;
flat in highp float vVisible;
flat
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
