#include <jni.h>
#include <GLES2/gl2.h>
#include <android/asset_manager_jni.h>
#include <string>
#include <assert.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <android/log.h>

#define  LOG_TAG    "opengl_test"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

using std::string;

GLfloat vVertices[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f,-0.5f, 0.0f
};
GLfloat vColors[] = {
        1.0f,1.0f,0.0f,
        0.0f,1.0f,0.0f,
        0.0f,0.0f,1.0f
};


std::string vertexShaderSource;
std::string fragmentShaderSource;

GLuint shaderProgram;

glm::mat4 ProjectionMatrix;
glm::mat4 ViewMatrix = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 5.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
);
glm::mat4 ModelMatrix = glm::mat4(1.0);

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

GLuint loadShader(GLenum shaderType, const std::string& pSource) {
        GLuint shader = glCreateShader(shaderType);
        assert(shader != 0);
        const char *sourceBuf = pSource.c_str();
        glShaderSource(shader, 1, &sourceBuf, NULL);
        glCompileShader(shader);
        GLint shaderCompiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
        assert(shaderCompiled != 0);
        LOGI("shader loaded");
        return shader;
}

GLuint createProgram(const std::string& pVertexSource, const std::string& pFragmentSource) {
        GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
        GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
        GLuint program = glCreateProgram();
        assert(program != 0);
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);
        glLinkProgram(program);
        GLint programLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &programLinked);
        assert(programLinked != 0);
        glDeleteShader(vertexShader);
        glDeleteShader(pixelShader);
        LOGI("program created");
        return program;
}


void init(AAssetManager *nativeAssetManager){
    loadSources(nativeAssetManager);
    shaderProgram = createProgram(vertexShaderSource,fragmentShaderSource);
    glUseProgram(shaderProgram);
    LOGI("program init");
}
void on_surface_created(){
    LOGI("on_surface_created");
    glClearColor(0.0f,0.0f,0.0f,1.0f);
}
void on_surface_changed(float width,float height){
    LOGI("on_surface_changed");
    ProjectionMatrix = glm::perspective(glm::radians(45.0f), width/height, 0.1f, 100.0f);
}

void on_draw(){
    LOGI("on_draw");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    ModelMatrix = glm::rotate(ModelMatrix,glm::degrees(0.0003f),glm::vec3(0.0f,0.0f,1.0f));
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix *  ModelMatrix;
    GLint MVP_Handle = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(MVP_Handle, 1, GL_FALSE, glm::value_ptr(MVP));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, vColors);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}



extern "C" {
    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_init(JNIEnv *env, jclass type, jobject assetManager) {
        AAssetManager *nativeAssetManager = AAssetManager_fromJava(env, assetManager);
        init(nativeAssetManager);
    }

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_on_1surface_1created(JNIEnv *env, jclass type) {
        on_surface_created();
    }

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_on_1surface_1changed(JNIEnv *env, jclass type, jint width, jint height) {
        float fwidth = width;
        float fheight = height;
        on_surface_changed(fwidth,fheight);
    }

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_on_1draw_1frame(JNIEnv *env, jclass type) {
        on_draw();
    }
}
