package com.example.nativegl;

import android.content.res.AssetManager;

public class NativeRenderer{
    static {
        System.loadLibrary("game");
    }
    public static native void on_surface_created();

    public static native void on_surface_changed(int width, int height);

    public static native void on_draw_frame();

    public static native void init(AssetManager assetManager);

    public static native void clean();

    public static native void r_on_surface_created();

    public static native void r_on_surface_changed(int width, int height);

    public static native void r_on_draw_frame();

    public static native void r_init(AssetManager assetManager);

    public static native void model_on_surface_created();

    public static native void model_on_surface_changed(int width, int height);

    public static native void model_on_draw_frame();

    public static native void model_init(AssetManager assetManager);

}
