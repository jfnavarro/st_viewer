#version 120

// vertexs
attribute highp vec4 qt_MultiTexCoord0;
attribute highp vec4 qt_Color;
attribute highp vec4 qt_Vertex;
uniform mediump mat4 qt_ModelViewMatrix;
uniform mediump mat4 qt_ModelViewProjectionMatrix;

// custom attributes
//attribute int in_options;
//attribute int in_values;
//attribute int in_references;

// passed along to fragment shader
varying highp vec4 textCoord;
varying highp vec4 outColor;
//varying highp vec2 out_options;

// uniform variables
//uniform int in_colorMode;
//uniform int in_geneMode;
//uniform int in_hitCountMin;
//uniform int in_hitCountMax;
//uniform float in_intensity;
//uniform int in_upper;
//uniform int in_lower;

// material
struct qt_MaterialParameters {
    mediump vec4 emission;
    mediump vec4 ambient;
    mediump vec4 diffuse;
    mediump vec4 specular;
    mediump float shininess;
};
uniform qt_MaterialParameters qt_Material;


float norm(float v, float t0, float t1)
{
    float vh = clamp(v, t0, t1);
    return (vh - t0) / (t1 - t0);
}

float denorm(float nv, float t0, float t1)
{
    float vh = clamp(nv, 0.0, 1.0);
    return (vh * (t1 - t0)) + t0;
}

vec4 createHeatMapColor(float wavelength)
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

float computeDynamicRangeAlpha(float value, float min_Value, float max_value)
{
    return sqrt(norm(value, min_Value, max_value));
}

void main(void)
{

    outColor = qt_Color;
    textCoord = qt_MultiTexCoord0;
	//out_options = in_options;
    //out_options = false;
    
    /*
    // input parameters to compute color
    float min_value = float(in_hitCountMin);
    float max_value = float(in_hitCountMax);
    int geneMode = int(in_geneMode);
    int colorMode = int(in_colorMode);
    float value = float(in_values);
    float references = float(in_references);
    float upper_limit = float(in_upper);
    float lower_limit = float(in_lower);
    
    //adjust color for globalMode
    if (geneMode == 1) {
        if (colorMode == 0) {
            //out_color.a = in_intensity;
        }
        else if (colorMode == 1) {
            out_color.a = computeDynamicRangeAlpha(value, min_value, max_value) + (1.0 - in_intensity);
        }
        else if (colorMode == 2) {
            float nv = norm(value, min_value, max_value);
            float wavel = sqrt(myclamp(nv, 0.0, 1.0));
            float nt = denorm(wavel, 380.0, 780.0);
            out_color = createHeatMapColor(nt);
            //out_color.a = in_intensity;
        }
        else {
            //error
        }
        
        if ( (value < lower_limit || value > upper_limit) && colorMode != 1) {
            //out_color.a = 0.0;
        }
        
        
    }
    else if (geneMode == 0) {
        if (colorMode == 1) {
            out_color.a += (1.0 - in_intensity);
        }
        else if ( out_color.a != 0.0 ){
            out_color.a = in_intensity;
        }
    }
    */
    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
}
