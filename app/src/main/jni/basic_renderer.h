#include <assert.h>
#include <android/log.h>

#define  LOG_TAG    "NATIVE_LOG"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


class BasicRenderer{
protected:
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    GLuint shaderProgram;
public:
    virtual void prepareData()=0;
    virtual void cleanData()=0;
    virtual void createViewMatrix()=0;
    virtual void loadSources(AAssetManager *assetManager)=0;
    virtual GLuint loadShader(GLenum shaderType, const std::string& pSource){
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
    };
    virtual GLuint createProgram(const std::string& pVertexSource, const std::string& pFragmentSource){
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
    };

    virtual void init(AAssetManager *nativeAssetManager)=0;
    virtual void on_surface_created()=0;
    virtual void on_surface_changed(float width,float height)=0;
    virtual void on_draw()=0;
};