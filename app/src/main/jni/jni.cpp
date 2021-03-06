#include <jni.h>
#include <android/asset_manager_jni.h>
#include "reverse_renderer.h"
#include "model_renderer.h.h"


extern "C" {
    Renderer renderer;
    ReverseRenderer r_renderer;
    ModelRenderer m_renderer;
    //jstring view_type;

    JNIEXPORT void JNICALL Java_com_example_nativegl_NativeRenderer_init(JNIEnv *env, jclass type, jobject assetManager) {
        AAssetManager *nativeAssetManager = AAssetManager_fromJava(env, assetManager);
        //view_type = view;
        renderer.init(nativeAssetManager);
    }
    JNIEXPORT void JNICALL Java_com_example_nativegl_NativeRenderer_on_1surface_1created(JNIEnv *env, jclass type) {
        renderer.on_surface_created();
    }

    JNIEXPORT void JNICALL Java_com_example_nativegl_NativeRenderer_on_1surface_1changed(JNIEnv *env, jclass type, jint width, jint height) {
        float fwidth = width;
        float fheight = height;
        renderer.on_surface_changed(fwidth,fheight);
    }

    JNIEXPORT void JNICALL Java_com_example_nativegl_NativeRenderer_on_1draw_1frame(JNIEnv *env, jclass type) {
//        const auto stringClass = env->FindClass("java/lang/String");
//        const auto equals = env->GetMethodID(stringClass, "equals", "()[B");
//        const auto main = env->NewStringUTF("main");
//        jboolean res = env->CallBooleanMethod(view_type, equals, main);
       // if(view_type!= nullptr){
        renderer.on_draw();
        //}else{
         //   rrenderer.on_draw();
        //}

    }
    JNIEXPORT void JNICALL Java_com_example_nativegl_NativeRenderer_clean(JNIEnv *env, jclass type) {
        renderer.cleanData();
    }

    JNIEXPORT void JNICALL Java_com_example_nativegl_NativeRenderer_r_1on_1surface_1created(JNIEnv *env, jclass type) {
        r_renderer.on_surface_created();
    }
    JNIEXPORT void JNICALL Java_com_example_nativegl_NativeRenderer_r_1on_1surface_1changed(JNIEnv *env, jclass type,
                                                                 jint width, jint height) {
        r_renderer.on_surface_changed(width,height);
    }

    JNIEXPORT void JNICALL Java_com_example_nativegl_NativeRenderer_r_1on_1draw_1frame(JNIEnv *env, jclass type) {
        r_renderer.on_draw();
    }

    JNIEXPORT void JNICALL Java_com_example_nativegl_NativeRenderer_r_1init(JNIEnv *env, jclass type, jobject assetManager) {
        AAssetManager *nativeAssetManager = AAssetManager_fromJava(env, assetManager);
        r_renderer.init(nativeAssetManager);
    }

JNIEXPORT void JNICALL
Java_com_example_nativegl_NativeRenderer_model_1on_1surface_1created(JNIEnv *env, jclass type) {

    m_renderer.on_surface_created();

}

JNIEXPORT void JNICALL
Java_com_example_nativegl_NativeRenderer_model_1on_1surface_1changed(JNIEnv *env, jclass type,
                                                                     jint width, jint height) {

    m_renderer.on_surface_changed(width,height);

}

JNIEXPORT void JNICALL
Java_com_example_nativegl_NativeRenderer_model_1on_1draw_1frame(JNIEnv *env, jclass type) {

    m_renderer.on_draw();

}

JNIEXPORT void JNICALL
Java_com_example_nativegl_NativeRenderer_model_1init(JNIEnv *env, jclass type,
                                                     jobject assetManager) {

    AAssetManager *nativeAssetManager = AAssetManager_fromJava(env, assetManager);
    m_renderer.init(nativeAssetManager);

}

}
