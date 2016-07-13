#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GLES3/gl3.h>
#include <vector>
#include <string>
#include <android/asset_manager.h>
#include <tiny_obj_loader/tiny_obj_loader.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#define  LOG_TAG    "NATIVE_LOG"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

using std::vector;
using std::string;

class ModelRenderer:public BasicRenderer{
public:
    AAssetManager *nativeAssetManager;

    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix = glm::lookAt(
            glm::vec3(0.0f, 10.0f, 40.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
    );
    glm::mat4 ModelMatrix = glm::mat4(1.0);

    struct Light {
        glm::vec3 position;
        glm::vec3 intensities; //a.k.a. the color of the light
    };

    Light gLight;

    vector<float> vb_model;

    void loadModel(){
        AAsset *modelAsset = AAssetManager_open(nativeAssetManager, "Bunny-LowPoly.obj",
                                                AASSET_MODE_BUFFER);
        assert(modelAsset != NULL);
        const void *modelBuff = AAsset_getBuffer(modelAsset);
        assert(modelBuff != NULL);
        off_t modelLength = AAsset_getLength(modelAsset);
        string modelSource = std::string((const char*) modelBuff,
                                         (size_t) modelLength);
        AAsset_close(modelAsset);

        vb_model.clear();
        std::istringstream issModel(modelSource);
        tinyobj::attrib_t attrib;
        vector<tinyobj::shape_t> shapes;
        vector<tinyobj::material_t> materials;

        string err;
        tinyobj::MaterialFileReader  readMatFn("");
        bool ret = tinyobj::LoadObj(&attrib,&shapes,&materials, &err, &issModel,&readMatFn,true);

        if (ret) LOGI("Model loaded succesfully.");

        for (size_t s = 0; s < shapes.size(); s++) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                int fv = shapes[s].mesh.num_face_vertices[f];

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    float vx = attrib.vertices[3*idx.vertex_index+0];
                    float vy = attrib.vertices[3*idx.vertex_index+1];
                    float vz = attrib.vertices[3*idx.vertex_index+2];
                    float nx = attrib.normals[3*idx.normal_index+0];
                    float ny = attrib.normals[3*idx.normal_index+1];
                    float nz = attrib.normals[3*idx.normal_index+2];
                    //float tx = attrib.texcoords[2*idx.texcoord_index+0];
                    //float ty = attrib.texcoords[2*idx.texcoord_index+1];

                    vb_model.push_back(vx);
                    vb_model.push_back(vy);
                    vb_model.push_back(vz);

                    vb_model.push_back(1.0f);//(float)(rand() % 256 +1)/256);
                    vb_model.push_back(1.0f);//(float)(rand() % 256 +1)/256);
                    vb_model.push_back(1.0f);//(float)(rand() % 256 +1)/256);

                    vb_model.push_back(nx);
                    vb_model.push_back(ny);
                    vb_model.push_back(nz);

                }
                index_offset += fv;
            }
        }

        ModelMatrix = glm::mat4(1.0f);

        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -50.0f, 0.0f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::degrees(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    void renderModel(){
        GLuint model_vbo = 0;
        glGenBuffers(1, &model_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, model_vbo);
        glBufferData(GL_ARRAY_BUFFER, vb_model.size() * sizeof(float), &vb_model.at(0), GL_STATIC_DRAW);

        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, model_vbo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), NULL);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(6 * sizeof(float)));


        GLint uniModel = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(ModelMatrix));

        glm::mat3 transinvModelMatrix = glm::transpose(glm::inverse(glm::mat3(ModelMatrix)));
        GLint unitrinvModel = glGetUniformLocation(shaderProgram, "transinvModel");
        glUniformMatrix3fv(unitrinvModel, 1, GL_FALSE, glm::value_ptr(transinvModelMatrix));


        GLint uniLigthPos = glGetUniformLocation(shaderProgram, "light.position");
        glUniform3fv(uniLigthPos, 1, glm::value_ptr(gLight.position));
        GLint uniLigthInt = glGetUniformLocation(shaderProgram, "light.intensities");
        glUniform3fv(uniLigthInt, 1, glm::value_ptr(gLight.intensities));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glDrawArrays(GL_TRIANGLES, 0, vb_model.size());

        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);

        glDeleteBuffers(1,&model_vbo);
    }
    void loadShaderSource(AAssetManager *assetManager) {
        AAsset *vertexShaderAsset = AAssetManager_open(assetManager, "model_vertex_shader.glsl",
                                                       AASSET_MODE_BUFFER);
        assert(vertexShaderAsset != NULL);
        const void *vertexShaderBuf = AAsset_getBuffer(vertexShaderAsset);
        assert(vertexShaderBuf != NULL);
        off_t vertexShaderLength = AAsset_getLength(vertexShaderAsset);
        vertexShaderSource = std::string((const char*)vertexShaderBuf,
                                         (size_t)vertexShaderLength);
        AAsset_close(vertexShaderAsset);

        AAsset *fragmentShaderAsset = AAssetManager_open(assetManager, "model_fragment_shader.glsl",
                                                         AASSET_MODE_BUFFER);
        assert(fragmentShaderAsset != NULL);

        const void *fragmentShaderBuf = AAsset_getBuffer(fragmentShaderAsset);
        assert(fragmentShaderBuf != NULL);
        off_t fragmentShaderLength = AAsset_getLength(fragmentShaderAsset);
        fragmentShaderSource = std::string((const char*)fragmentShaderBuf,
                                           (size_t)fragmentShaderLength);
    }

    void init(AAssetManager *nativeAssetManager) {
        loadShaderSource(nativeAssetManager);
        shaderProgram = createProgram(vertexShaderSource, fragmentShaderSource);
        glUseProgram(shaderProgram);
        loadModel();
    }
    void on_surface_created() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        gLight.position = glm::vec3(0.0f, 10.0f, 40.0f);
        gLight.intensities = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    void on_surface_changed(float width, float height) {
        float fwidth = width;
        float fheight = height;

        ProjectionMatrix = glm::perspective(glm::radians(45.0f), fwidth/fheight, 0.1f, 100.0f);
    }

    void on_draw() {
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        ModelMatrix = glm::rotate(ModelMatrix,glm::degrees(0.0003f),glm::vec3(0.0f,0.0f,1.0f));
        glm::scale(ModelMatrix, glm::vec3(0.5,0.5,0.5));
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix *  ModelMatrix;
        GLint MVP_Handle = glGetUniformLocation(shaderProgram, "MVP");
        glUniformMatrix4fv(MVP_Handle, 1, GL_FALSE, glm::value_ptr(MVP));
        renderModel();
    }

    virtual void prepareData() override ;

    virtual void cleanData() override ;

    virtual void createViewMatrix() override ;

    virtual void loadSources(AAssetManager *assetManager) override ;
};


void ModelRenderer::prepareData() {

}

void ModelRenderer::cleanData() {

}

void ModelRenderer::createViewMatrix() {

}

void ModelRenderer::loadSources(AAssetManager *assetManager) {

}
