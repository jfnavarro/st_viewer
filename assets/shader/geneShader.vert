#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in int selected;
layout(location = 3) in int visible;

out vec4 vColor;
//out float vVisible;
//out float vSelected;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;
uniform int size;
uniform float alpha;

void main()
{
    gl_Position = cameraToView * worldToCamera * modelToWorld * vec4(position, 1.0);
    gl_PointSize = size;
    vColor = color;
    vColor.a = bool(visible) ? alpha : 0.0;
    //vVisible = float(visible);
    //vSelected = float(selected);
}
