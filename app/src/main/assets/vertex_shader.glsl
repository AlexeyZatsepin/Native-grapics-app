#version 100
attribute vec3 vertexCoords;
attribute vec3 vertexColor;

uniform mat4 MVP;

varying vec3 fColor;

void main(){
    gl_Position = MVP * vec4(vertexCoords,1.0);
    fColor = vertexColor;
    gl_PointSize = 5.0;
}