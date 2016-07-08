#include <jni.h>
#include <GLES2/gl2.h>
#include <android/asset_manager_jni.h>
#include <string>
#include <assert.h>


const GLfloat vVertices[] = {
        -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
//
        0.0f,1.0f,1.0f, 0.0f, 0.0f,
        1.0f,1.0f, 0.0f, 1.0f, 0.0f,
        1.0f,0.0f, 0.0f, 0.0f, 1.0f,
//
        1.0f,0.0f,1.0f, 0.0f, 0.0f,
        1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
        0.0f,-1.0f, 0.0f, 0.0f, 1.0f,
//
        0.0f,-1.0f,1.0f, 0.0f, 0.0f,
        -1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f,0.0f, 0.0f, 1.0f,

        //line
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f ,0.0f, 0.0f, 1.0f,
        0.0f ,-0.5f,0.0f, 0.0f, 1.0f,
        -0.5f, 0.0f ,0.0f, 0.0f, 1.0f,
};


struct Renderer {
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

    GLuint shaderProgram;
    GLuint aPositionLocation;
    GLuint aColorLocation;

    public:
    void init(AAssetManager *assetManager) {
        AAsset *vertexShaderAsset = AAssetManager_open(assetManager, "vertex_shader.glsl",
                                                       AASSET_MODE_BUFFER);
        assert(vertexShaderAsset != NULL);
        const void *vertexShaderBuf = AAsset_getBuffer(vertexShaderAsset);
        assert(vertexShaderBuf != NULL);
        off_t vertexShaderLength = AAsset_getLength(vertexShaderAsset);
        vertexShaderSource = std::string((const char*)vertexShaderBuf,
                                         (size_t)vertexShaderLength);
        AAsset_close(vertexShaderAsset);

        AAsset *fragmentShaderAsset = AAssetManager_open(assetManager, "fragment_shader.glsl",
                                                         AASSET_MODE_BUFFER);
        assert(fragmentShaderAsset != NULL);
        const void *fragmentShaderBuf = AAsset_getBuffer(fragmentShaderAsset);
        assert(fragmentShaderBuf != NULL);
        off_t fragmentShaderLength = AAsset_getLength(fragmentShaderAsset);
        fragmentShaderSource = std::string((const char*)fragmentShaderBuf,
                                           (size_t)fragmentShaderLength);
        AAsset_close(fragmentShaderAsset);

        glUseProgram(shaderProgram);

        glVertexAttribPointer(aPositionLocation, 2, GL_FLOAT, (GLboolean) false, 20, vVertices);
        glEnableVertexAttribArray(aPositionLocation);


        glVertexAttribPointer(aColorLocation, 3, GL_FLOAT, (GLboolean) false, 20, vVertices);
        glEnableVertexAttribArray(aColorLocation);

    }

    void surfaceCreated() {
        shaderProgram = createProgram(vertexShaderSource, fragmentShaderSource);
        assert(shaderProgram != 0);
        aPositionLocation = (GLuint)glGetAttribLocation(shaderProgram, "a_Position");
        assert(aPositionLocation != -1);
        aColorLocation = (GLuint)glGetAttribLocation(shaderProgram, "a_Color");
        assert(aColorLocation != -1);
    }

    void surfaceChanged(int w, int h) {
        glViewport(0, 0, w, h);
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
        return program;
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
        return shader;
    }


    void render() {
        glClearColor(0.f, 0.f, 0.f, 0.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 3, 3);
        glDrawArrays(GL_TRIANGLES, 6, 3);
        glDrawArrays(GL_TRIANGLES, 9, 3);

//        glDrawArrays(GL_TRIANGLES, 0, 12);
        glLineWidth(12);

        glDrawArrays(GL_LINES, 12, 2);
        glDrawArrays(GL_LINES, 14, 2);

        glDrawArrays(GL_POINTS,16,4);


    }

};
extern "C" {

    Renderer renderer;

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_init(JNIEnv *env, jclass type, jobject assetManager) {
        AAssetManager *nativeAssetManager = AAssetManager_fromJava(env, assetManager);
        renderer.init(nativeAssetManager);
    }

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_on_1surface_1created(JNIEnv *env, jclass type) {
        renderer.surfaceCreated();

    }

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_on_1surface_1changed(JNIEnv *env, jclass type, jint width, jint height) {
        renderer.surfaceChanged(width,height);
    }

    JNIEXPORT void JNICALL Java_com_example_hellojni_NativeRenderer_on_1draw_1frame(JNIEnv *env, jclass type) {
        renderer.render();
    }

}
