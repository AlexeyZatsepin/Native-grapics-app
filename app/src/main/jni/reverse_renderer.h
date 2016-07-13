#include "renderer.h"
#include <android/asset_manager.h>

class ReverseRenderer: public Renderer{
public:
    void drawTriangles(){
        ModelMatrix = prevModelMatrix;
        ModelMatrix = glm::rotate(ModelMatrix,glm::degrees(0.0003f),glm::vec3(0.0f,-1.0f,0.0f));
        bindMatrix();
        glDrawArrays(GL_TRIANGLES, 0, 12);
        prevModelMatrix = ModelMatrix;
    }
};
