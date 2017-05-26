#version 120

// graphic data
attribute lowp vec4 colorAttr;
attribute lowp vec2 textureAttr;
attribute highp vec4 vertexAttr;

// model_view * projection matrix
uniform mediump mat4 in_ModelViewProjectionMatrix;

// passed along to fragment shader
varying highp vec2 outTextCoord;
varying lowp vec4 outColor;

void main(void)
{
    outColor = colorAttr;
    outTextCoord = textureAttr;
    gl_Position = in_ModelViewProjectionMatrix * vertexAttr;
}
