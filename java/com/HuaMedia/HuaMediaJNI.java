/*
* 时间：2017-1-18
* 创造者：ChenXingHua
* 脚本：JNI调用c++ opengGL渲染代码
* */
package com.HuaMedia;

import android.util.Log;

/**
 * Created by Tuzi on 2017/1/18.
 */
public class HuaMediaJNI {

    public static boolean loadLibrary(String nLibName) {
        try {
            System.loadLibrary(nLibName);
            Log.i("xinghua","Native library lib" + nLibName + ".so loaded");
            return true;
        } catch (UnsatisfiedLinkError var2) {
            Log.i("xinghua","The library lib" + nLibName + ".so could not be loaded");
        } catch (SecurityException var3) {
            Log.i("xinghua","The library lib" + nLibName + ".so was not allo wed to be loaded");
        }
        return false;
    }
    //初始化opengl版本
    public  native void native_InitNative(int OpengGLVersion);
    //创建数据承载纹理
    public  native int native_CreateOESTexture();
    //创建FBO离屏渲染
    public  native void native_CreateFBO(int[] textureids,int width,int height);
    //渲染
    public  native void native_RenderTextrue(float[] rendMax);
    //视频普通格式
    public  native void  native_CommonFormat();
    //视频左右格式
    public  native void  native_LeftRightFormat();
    //视频格式上下
    public native  void  native_UpDownFormat();
    //释放
    public  native  void  Native_Release();
}
