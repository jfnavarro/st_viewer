#version 330

in highp vec4 vColor;
in highp float vVisible;
in highp float vSelected;

out highp vec4 fColor;

void main()
{
    if (!bool(vVisible)) {
        discard;
    }
    
    if (!bool(vSelected)) {
        float r = 0.0, delta = 0.0, alpha = 1.0;
        vec2 cxy = 2.0 * gl_PointCoord - 1.0;
        r = dot(cxy, cxy);
        delta = fwidth(r);
        alpha = 1.0 - smoothstep(1.0 - delta, 1.0 + delta, r);
        fColor = vColor * alpha;
    } else {
        fColor = vColor;
    }
}
