package com.example.hellojni;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;


public class MainActivity extends Activity {
    private GLSurfaceView glSurfaceView;
    private boolean rendererSet;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        glSurfaceView = new GLSurfaceView(this);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.FROYO) {
            glSurfaceView.setEGLContextClientVersion(2);
        }
        glSurfaceView.setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                NativeRenderer.on_surface_created();
            }

            @Override
            public void onSurfaceChanged(GL10 gl, int width, int height) {
                NativeRenderer.on_surface_changed(width,height);
            }

            @Override
            public void onDrawFrame(GL10 gl) {
                NativeRenderer.on_draw_frame();
            }
        });
        NativeRenderer.init(getAssets());
        rendererSet = true;
        setContentView(glSurfaceView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (rendererSet) {
            glSurfaceView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (rendererSet) {
            glSurfaceView.onResume();
        }
    }
}
