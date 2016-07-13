#version 300 es
in vec3 vertexCoords;
in vec3 vertexColor;
in vec3 vertexNormal;

uniform mat4 MVP;

out vec3 fColor;
out vec3 fragNormal;
out vec3 fragVert;
void main(){

    gl_Position = MVP * vec4(vertexCoords,1.0);

    fColor = vertexColor;
    fragNormal = vertexNormal;
    fragVert = vertexCoords;
}