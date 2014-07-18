#version 120

// vertexs
attribute lowp vec4 qt_MultiTexCoord0;
attribute lowp vec4 qt_Color;
attribute highp vec4 qt_Vertex;
attribute lowp vec4 qt_Custom0;
attribute lowp float qt_Custom1;
uniform mediump mat4 qt_ModelViewMatrix;
uniform mediump mat4 qt_ModelViewProjectionMatrix;

// passed along to fragment shader
varying highp vec4 textCoord;
varying lowp vec4 outColor;
varying lowp float outSelected;
varying lowp float outShape;

// uniform variables
uniform lowp int in_visualMode;
uniform lowp int in_pooledUpper;
uniform lowp int in_pooledLower;
uniform lowp int in_shape;
uniform lowp float in_intensity;
uniform lowp float in_shine;

//Some in-house functions

float norm(inout float v, in float t0, in float t1)
{
    float vh = clamp(v, t0, t1);
    return (vh - t0) / (t1 - t0);
}

float denorm(inout float nv, in float t0, in float t1)
{
    float vh = clamp(nv, 0.0, 1.0);
    return (vh * (t1 - t0)) + t0;
}

vec4 createHeatMapColor(inout float wavelength)
{
    float gamma = 0.8;
    // clamp input value
    float cwavelength = clamp(wavelength, 380.0, 780.0);
    
    // define colors according to wave lenght spectra
    float red;
    float green;
    float blue;
    if (380.0 <= cwavelength && cwavelength < 440.0) {
        red = -(cwavelength - 440.0) / (440.0 - 380.0);
        green = 0.0;
        blue = 1.0;
    } else if (440.0 <= cwavelength && cwavelength < 490.0) {
        red = 0.0;
        green = (cwavelength - 440.0) / (490.0 - 440.0);
        blue = 1.0;
    } else if (490.0 <= cwavelength && cwavelength < 510.0) {
        red = 0.0;
        green = 1.0;
        blue = -(cwavelength - 510.0) / (510.0 - 490.0);
    } else if (510.0 <= cwavelength && cwavelength < 580.0) {
        red = (cwavelength - 510.0) / (580.0 - 510.0);
        green = 1.0;
        blue = 0.0;
    } else if (580.0 <= cwavelength && cwavelength < 645.0) {
        red = 1.0;
        green = -(cwavelength - 645.0) / (645.0 - 580.0);
        blue = 0.0;
    } else if (645.0 <= cwavelength && cwavelength <= 780.0) {
        red = 1.0;
        green = 0.0;
        blue = 0.0;
    } else {
        red = 0.0;
        green = 0.0;
        blue = 0.0;
    }
    // float the intensity fall off near the vision limits
    float factor;
    if (380.0 <= cwavelength && cwavelength < 420.0) {
        factor = 0.3 + 0.7 * (cwavelength - 380.0) / (420.0 - 380.0);
    } else if (420.0 <= cwavelength && cwavelength < 700.0) {
        factor = 1.0;
    } else if (700.0 <= cwavelength && cwavelength <= 780.0) {
        factor = 0.3 + 0.7 * (780.0 - cwavelength) / (780.0 - 700.0);
    } else {
        factor = 0.3;
    }
    // Gamma adjustments (clamp to [0.0, 1.0])
    red = clamp(pow(red * factor, gamma), 0.0, 1.0);
    green = clamp(pow(green * factor, gamma), 0.0, 1.0);
    blue = clamp(pow(blue * factor, gamma), 0.0, 1.0);
    // return color
    return vec4(red, green, blue, 1.0);
}

float computeDynamicRangeAlpha(inout float value, in float min_Value, in float max_value)
{
    return sqrt(norm(value, min_Value, max_value));
}

void main(void)
{
    outColor = qt_Color;
    outSelected = float(qt_Custom0);
    textCoord = qt_MultiTexCoord0;
    outShape = float(in_shape);
    
    int geneMode = int(in_visualMode);
    float value = float(qt_Custom1);
    float upper_limit = float(in_pooledUpper);
    float lower_limit = float(in_pooledLower);
    float shine = float(in_shine);
    
    // is visible?
    if (value == 0.0) {
        outColor.a = 0.0;
    }
    else if (geneMode == 1) {
        outColor.a =
            computeDynamicRangeAlpha(value, lower_limit, upper_limit) + (1.0 - in_intensity);
    }
    else if (geneMode == 2) {
        float nv = norm(value, lower_limit, upper_limit);
        float wavel = sqrt(clamp(nv, 0.0, 1.0));
        float nt = denorm(wavel, 380.0, 780.0);
        outColor = createHeatMapColor(nt);
        outColor.a = in_intensity;
    }
    else {
        outColor.a = in_intensity;
    }
    
    
    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
}
