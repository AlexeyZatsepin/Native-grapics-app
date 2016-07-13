package com.example.nativegl;


import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;


public class MainActivity extends AppCompatActivity {
    private final String TAG = "CurrView";
    private GLSurfaceView glSurfaceView;
//    public enum Views{
//        main, reverse
//    }
//    Views view;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        glSurfaceView = new GLSurfaceView(this);
//        view = (Views) getIntent().getSerializableExtra(TAG);
//        if (view == null) view = Views.main;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.FROYO) {
            glSurfaceView.setEGLContextClientVersion(2);
        }
        glSurfaceView.setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                NativeRenderer.init(getAssets());
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
        setContentView(glSurfaceView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        glSurfaceView.onPause();
        //NativeRenderer.cleanData();

    }

    @Override
    protected void onResume() {
        super.onResume();
        glSurfaceView.onResume();
        //NativeRenderer.init(getAssets());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        NativeRenderer.clean();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
//        if (id == R.id.main) {
//            view = Views.main;
//        }else if(id==R.id.reverse){
//            view = Views.reverse;
//        }
//        Intent intent = getIntent();
//        intent.putExtra(TAG, view);
//        finish();
//        startActivity(intent);
        return super.onOptionsItemSelected(item);
    }

}
