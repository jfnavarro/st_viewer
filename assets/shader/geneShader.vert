#version 120

// graphic data
attribute lowp vec4 colorAttr;
attribute lowp vec2 textureAttr;
attribute highp vec4 vertexAttr;
attribute lowp float visibleAttr;
attribute lowp float selectedAttr;
attribute lowp float readsAttr;
uniform mediump mat4 in_ModelViewMatrix;
uniform mediump mat4 in_ModelViewProjectionMatrix;

// passed along to fragment shader
varying highp vec2 outTextCoord;
varying lowp vec4 outColor;
varying lowp float outSelected;
varying lowp float outShape;

// uniform variables
uniform lowp int in_visualMode;
uniform lowp int in_colorMode;
uniform lowp int in_poolingMode;
uniform lowp int in_pooledUpper;
uniform lowp int in_pooledLower;
uniform lowp int in_shape;
uniform lowp float in_intensity;

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
    outColor = colorAttr;
    outSelected = selectedAttr;
    outTextCoord = textureAttr;
    outShape = float(in_shape);
    
    //get the value attribute and limits (Reads, genes or TPM)
    float value = readsAttr;
    float upper_limit = float(in_pooledUpper);
    float lower_limit = float(in_pooledLower);
    
    //adjust for color mode (1 linear - 2 log - 3 exp)
    if (in_colorMode == 2) {
        value = log(value + 1);
        upper_limit = log(upper_limit + 1);
        lower_limit = log(lower_limit + 1);
    } else if (in_colorMode == 3) {
        value = sqrt(value);
        upper_limit = sqrt(upper_limit);
        lower_limit = sqrt(lower_limit);
    }
    
    if (bool(visibleAttr)) {
        //visual modes (1 normal - 2 dynamic range - 3 heatmap)
        outColor.a = in_intensity;
        if (in_visualMode == 2) { //dynamic range mode
            float normalizedValue = norm(value, lower_limit, upper_limit);
            outColor.a = normalizedValue + (1.0 - in_intensity);
        } else if (in_visualMode == 3) { //heat map mode
            float normalizedValue = norm(value, lower_limit, upper_limit);
            outColor = createHeatMapColor(normalizedValue);
            outColor.a = in_intensity;
        }
    } else {
        outColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
    
    gl_Position = in_ModelViewProjectionMatrix * vertexAttr;
}
