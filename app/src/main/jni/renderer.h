#include <GLES3/gl3.h>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utils.h"
#include "basic_renderer.h"
#include <android/log.h>

class Renderer: public BasicRenderer{
protected:
    GLfloat * vVertices;
    GLfloat * vColors;

    GLint MVP_Handle;

    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    glm::mat4 ModelMatrix;
    glm::mat4 prevModelMatrix;
public:
    void prepareData(){
        float s = 0.4f;
        float d = 0.9f;
        float l = 3;

        vVertices = new GLfloat[54]{
                // первый треугольник
                -2*s, -s, d,
                2*s, -s, d,
                0, s, d,

                // второй треугольник
                -2*s, -s, -d,
                2*s, -s, -d,
                0, s, -d,

                // третий треугольник
                d, -s, -2*s,
                d, -s, 2*s,
                d, s, 0,

                // четвертый треугольник
                -d, -s, -2*s,
                -d, -s, 2*s,
                -d, s, 0,

                // ось X
                -l, 0,0,
                l,0,0,

                // ось Y
                0, -l, 0,
                0 ,l,0,

                // ось Z
                0,0,-l,
                0,0,l,
        };
        vColors = new GLfloat[54]{
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,

                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,

                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,

                1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,

                0.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f,

                1.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 1.0f,

                1.0f, 0.5f, 0.0f,
                1.0f, 0.5f, 0.0f,
        };
    }
    void cleanData(){
        delete vVertices;
        delete vColors;
    }
    void createViewMatrix(){
//        float angle = now() * 2 * 3.1415f / 10000;
        // точка положения камеры
//    float eyeX = (float) (cos(angle)*5);
        float eyeX = 5;
        float eyeY = 2;
//    float eyeY = (float) (sin(angle)*5);
//    float eyeZ = (float) (sin(angle)*5);
        float eyeZ = 5;

        // точка направления камеры
        float centerX = 0;
        float centerY = 0;
        float centerZ = 0;

        // up-вектор
        float upX = 0;
        float upY = 1;
        float upZ = 0;
        ViewMatrix = glm::lookAt(
                glm::vec3(eyeX, eyeY, eyeZ),
                glm::vec3(centerX, centerY, centerZ),
                glm::vec3(upX, upY, upZ)
        );
    }

    void loadSources(AAssetManager *assetManager) {
        AAsset *vertexShaderAsset = AAssetManager_open(assetManager, "vertex_shader.glsl",
                                                       AASSET_MODE_BUFFER);
        assert(vertexShaderAsset != NULL);
        const void *vertexShaderBuf = AAsset_getBuffer(vertexShaderAsset);
        assert(vertexShaderBuf != NULL);
        off_t vertexShaderLength = AAsset_getLength(vertexShaderAsset);
        vertexShaderSource = std::string((const char*)vertexShaderBuf,
                                         (size_t)vertexShaderLength);
        AAsset_close(vertexShaderAsset);
        LOGI("vertex shader source loaded");
        AAsset *fragmentShaderAsset = AAssetManager_open(assetManager, "fragment_shader.glsl",
                                                         AASSET_MODE_BUFFER);
        assert(fragmentShaderAsset != NULL);
        const void *fragmentShaderBuf = AAsset_getBuffer(fragmentShaderAsset);
        assert(fragmentShaderBuf != NULL);
        off_t fragmentShaderLength = AAsset_getLength(fragmentShaderAsset);
        fragmentShaderSource = std::string((const char*)fragmentShaderBuf,
                                           (size_t)fragmentShaderLength);
        AAsset_close(fragmentShaderAsset);
        LOGI("fragment shader source loaded");
    }




    void init(AAssetManager *nativeAssetManager){
        prepareData();
        loadSources(nativeAssetManager);
        shaderProgram = createProgram(vertexShaderSource,fragmentShaderSource);
        glUseProgram(shaderProgram);
        LOGI("program init");
    }

    void on_surface_created(){
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glEnable(GL_DEPTH_TEST);

        ModelMatrix = glm::mat4(1.0);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, vColors);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        MVP_Handle = glGetUniformLocation(shaderProgram, "MVP");

        createViewMatrix();
    }

    void on_surface_changed(float width,float height){
        ProjectionMatrix = glm::perspective(glm::radians(45.0f), width/height, 0.1f, 100.0f);
    }

    void on_draw(){
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        createViewMatrix();
        drawAxes();
        drawTriangles();
    }
    virtual void bindMatrix(){
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(MVP_Handle, 1, GL_FALSE, glm::value_ptr(MVP));
    }
    virtual void drawAxes(){
        ModelMatrix = glm::mat4(1.0);
        bindMatrix();
        glLineWidth(6);
        glDrawArrays(GL_LINES, 12, 6);
    }
    virtual void drawTriangles(){
        ModelMatrix = prevModelMatrix;
        ModelMatrix = glm::rotate(ModelMatrix,glm::degrees(0.0003f),glm::vec3(0.0f,1.0f,0.0f));
        bindMatrix();
        glDrawArrays(GL_TRIANGLES, 0, 12);
        prevModelMatrix = ModelMatrix;
    }
};
