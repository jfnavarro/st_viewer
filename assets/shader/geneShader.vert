#version 120

// graphic data
attribute highp vec2 textureAttr;
attribute highp vec4 vertexAttr;
attribute mediump vec4 colorAttr;
attribute mediump float selectedAttr;

// model_view * projection matrix
uniform highp mat4 in_ModelViewProjectionMatrix;

// passed along to fragment shader
varying highp vec2 outTextCoord;
varying mediump vec4 outColor;
varying lowp float outSelected;

void main(void)
{
    outColor = colorAttr;
    outTextCoord = textureAttr;
    outSelected = selectedAttr;
    gl_Position = in_ModelViewProjectionMatrix * vertexAttr;
}
