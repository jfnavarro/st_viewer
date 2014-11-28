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
uniform lowp int in_colorMode;
uniform lowp int in_poolingMode;
uniform lowp float in_pooledUpper;
uniform lowp float in_pooledLower;
uniform lowp int in_shape;
uniform lowp float in_intensity;
uniform lowp int in_totalReads;

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

vec4 createHeatMapColor(inout float value)
{
    float gamma = 0.8;
    
    // denorm input to 380-780
    float cwavelength = denorm(value, 380.0, 780.0);
    
    // define colors according to wave lenght spectra
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
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
    }
    
    // float the intensity fall off near the vision limits
    float factor = 0.3;
    if (380.0 <= cwavelength && cwavelength < 420.0) {
        factor = 0.3 + 0.7 * (cwavelength - 380.0) / (420.0 - 380.0);
    } else if (420.0 <= cwavelength && cwavelength < 700.0) {
        factor = 1.0;
    } else if (700.0 <= cwavelength && cwavelength <= 780.0) {
        factor = 0.3 + 0.7 * (780.0 - cwavelength) / (780.0 - 700.0);
    }
    
    // Gamma adjustments (clamp to [0.0, 1.0])
    red = clamp(pow(red * factor, gamma), 0.0, 1.0);
    green = clamp(pow(green * factor, gamma), 0.0, 1.0);
    blue = clamp(pow(blue * factor, gamma), 0.0, 1.0);
    
    // return color
    return vec4(red, green, blue, 1.0);
}

void main(void)
{
    outColor = qt_Color;
    outSelected = float(qt_Custom0);
    textCoord = qt_MultiTexCoord0;
    outShape = float(in_shape);
    
    int visualMode = int(in_visualMode);
    int colorMode = int(in_colorMode);
    int poolingMode = int(in_poolingMode);
    float value = float(qt_Custom1);
    float upper_limit = float(in_pooledUpper);
    float lower_limit = float(in_pooledLower);
    float totalReads = float(in_totalReads);

    //if mode is TPM
    if (poolingMode == 2) {
        value = (value * 10e5) / totalReads;
        upper_limit = (upper_limit * 10e5) / totalReads;
        upper_limit = (upper_limit * 10e5) / totalReads;
    }
    
    //adjust for color mode (0 exp - 1 log - 2 linear)
    if (colorMode == 1) {
        value = log(value + 1);
        upper_limit = log(upper_limit + 1);
        lower_limit = log(lower_limit + 1);
    } else if (colorMode == 0) {
        value = sqrt(value);
        upper_limit = sqrt(upper_limit);
        lower_limit = sqrt(lower_limit);
    }
    
    //the color computation functions and helpers
    //are the same as the class HeatMap
    
    // if value is 0 the feature is not visible
    if (value == 0.0) {
        outColor.a = 0.0;
    } else if (visualMode == 1) { //dynamic range mode
        float normalizedValue = norm(value, lower_limit, upper_limit);
        outColor.a = normalizedValue + (1.0 - in_intensity);
    } else if (visualMode == 2) { //heat map mode
        float normalizedValue = norm(value, lower_limit, upper_limit);
        outColor = createHeatMapColor(normalizedValue);
        outColor.a = in_intensity;
    } else {
        outColor.a = in_intensity;
    }
    
    
    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
}
