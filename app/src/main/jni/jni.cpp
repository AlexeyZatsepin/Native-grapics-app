#include <jni.h>
#include <GLES3/gl3.h>
#include <android/asset_manager_jni.h>
#include <string>
#include <assert.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <android/log.h>

#define  LOG_TAG    "NATIVE_LOG"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

using std::string;


long long now() {
    struct timeval tp;
    gettimeofday(&tp,NULL);
    long long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;

}
struct Renderer{
    GLfloat * vVertices;
    GLfloat * vColors;

    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    GLuint shaderProgram;
    GLint MVP_Handle;

    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    glm::mat4 ModelMatrix;
    glm::mat4 prevModelMatrix;
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
void clean(){
    delete vVertices;
    delete vColors;
}
void createViewMatrix(){
    float angle = now() * 2 * 3.1415f / 10000;
    // точка положения камеры
//    float eyeX = (float) (cos(angle)*5);
    float eyeX = 5;
    float eyeY = 2;
//    float eyeY = (float) (sin(angle)*5);
    float eyeZ = (float) (sin(angle)*5);
//    float eyeZ = 5;

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
    void bindMatrix(){
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(MVP_Handle, 1, GL_FALSE, glm::value_ptr(MVP));
    }
    void drawAxes(){
        ModelMatrix = glm::mat4(1.0);
        bindMatrix();
        glLineWidth(6);
        glDrawArrays(GL_LINES, 12, 6);
    }
    void drawTriangles(){
        ModelMatrix = prevModelMatrix;
        ModelMatrix = glm::rotate(ModelMatrix,glm::degrees(0.0003f),glm::vec3(0.0f,1.0f,0.0f));
        bindMatrix();
        glDrawArrays(GL_TRIANGLES, 0, 12);
        prevModelMatrix = ModelMatrix;
    }
};



extern "C" {
    Renderer renderer;
JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_init(JNIEnv *env, jclass type, jobject assetManager) {
        AAssetManager *nativeAssetManager = AAssetManager_fromJava(env, assetManager);
        renderer.init(nativeAssetManager);
    }

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_on_1surface_1created(JNIEnv *env, jclass type) {
        renderer.on_surface_created();
    }

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_on_1surface_1changed(JNIEnv *env, jclass type, jint width, jint height) {
        float fwidth = width;
        float fheight = height;
        renderer.on_surface_changed(fwidth,fheight);
    }

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_on_1draw_1frame(JNIEnv *env, jclass type) {
        renderer.on_draw();
    }

    JNIEXPORT void JNICALL
    Java_com_example_hellojni_NativeRenderer_clean(JNIEnv *env, jclass type) {
        renderer.clean();
    }
}
