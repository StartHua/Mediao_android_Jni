/*
* 时间：2017-1-18
* 创造者：ChenXingHua
* 脚本：opengGL渲染代码
* */
#include<com_HuaMedia_HuaMediaJNI.h>
#include <jni.h>
#include <stdlib.h>
#include"shaderHelp.h"
#include"Utils.h"

//Superset of OGL2
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

//Needed for GL_TEXTURE_EXTERNAL_OES
#include <GLES2/gl2ext.h>

//视频格式
enum VideoFormat_enum{
    Common,
    LeftRigth,
    UpDown
};

//句柄
unsigned int shaderProgramID    = 0;
GLint vertexHandle              = 0;
GLint textureCoordHandle        = 0;
GLint mvpMatrixHandle           = 0;
GLint textureMatrixHandle       = 0;

//opengGL版本
int _glVersion = -1;

//投影矩阵
float orthoProjMatrix[16];
//顶点坐标
float orthoQuadVertices[] =
{
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f
};
//普通纹理坐标
float orthoQuadTexCoords[] =
{
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

//左右视频纹理坐标
float letfRightTexCoords[] =
{
    0.5f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    0.5f, 1.0f
};

//上下视频纹理坐标
float upDownTexCoords[]=
{
     1.0f, 0.0f,
     0.0f, 0.0f,
     0.0f, 0.5f,
     1.0f, 0.5f
};

//视频格式枚举
VideoFormat_enum videoFormat = Common;

//绘制顺序
unsigned char orthoQuadIndices[]=
{
    0, 1, 2, 2, 3, 0
};

//java 视频的SurfaceTexture 使用的OpenGL纹理id
GLuint mediaTextureID ;
//渲染纹理ids
//jint* textureidArr ;
//纹理数组长度
jint ArrLength;
//FBO离屏渲染句柄
GLuint fbo = 0;
//高宽
jint Width;
jint Height;

bool
setOrthographicProjectionMatrix(float orthoMatrix[16])
{
    // Set projection matrix for orthographic projection:
    for (int i = 0; i < 16; i++) orthoMatrix[i] = 0.0f;
    float nLeft   = -1.0;
    float nRight  =  1.0;
    float nBottom = -1.0;
    float nTop    =  1.0;
    float nNear   = -1.0;
    float nFar    =  1.0;

    orthoMatrix[0]  =  2.0f / (nRight - nLeft);
    orthoMatrix[5]  =  2.0f / (nTop - nBottom);
    orthoMatrix[10] =  2.0f / (nNear - nFar);
    orthoMatrix[12] = -(nRight + nLeft) / (nRight - nLeft);
    orthoMatrix[13] = -(nTop + nBottom) / (nTop - nBottom);
    orthoMatrix[14] =  (nFar + nNear) / (nFar - nNear);
    orthoMatrix[15] =  1.0f;

    return true;
}



JNIEXPORT void JNICALL Java_com_HuaMedia_HuaMediaJNI_native_1InitNative
  (JNIEnv *, jobject,jint openGLVersion)
  {
        _glVersion = openGLVersion;
  }


JNIEXPORT jint JNICALL Java_com_HuaMedia_HuaMediaJNI_native_1CreateOESTexture
  (JNIEnv *, jobject)
  {
        //生成纹理并且绑定到GL_TEXTURE_EXTERNAL_OES
        glGenTextures(1, &mediaTextureID);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, mediaTextureID);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
        return mediaTextureID;
  }


JNIEXPORT void JNICALL Java_com_HuaMedia_HuaMediaJNI_native_1CreateFBO
  (JNIEnv * env, jobject, jintArray Attr, jint videoWidth, jint videoHeight)
  {
        //c++ java ndk 数组转换(最后记得释放)
        jint* textureidArr = env->GetIntArrayElements(Attr, 0);
        ArrLength = env->GetArrayLength(Attr);
        LOG("textureidArr  %d, ArrLength: %d", textureidArr[0], ArrLength );

        Width = videoWidth;
        Height = videoHeight;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,textureidArr[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoWidth, videoHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0);

        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,textureidArr[0], 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shaderProgramID     = shaderHelp::createProgramFromBuffer(VertexShader,
                                                               FragmentShader);
        vertexHandle        = glGetAttribLocation(shaderProgramID,
                                              "vertexPosition");
        textureCoordHandle  = glGetAttribLocation(shaderProgramID,
                                              "vertexTexCoord");
        mvpMatrixHandle     = glGetUniformLocation(shaderProgramID,
                                               "modelViewProjectionMatrix");
        textureMatrixHandle = glGetUniformLocation(shaderProgramID,
                                               "textureMatrix");

        setOrthographicProjectionMatrix(orthoProjMatrix);

        env->ReleaseIntArrayElements(Attr,textureidArr,0); //释放数组

        shaderHelp::checkGlError("VuforiaMedia initFBO");
  }


JNIEXPORT void JNICALL Java_com_HuaMedia_HuaMediaJNI_native_1RenderTextrue
  (JNIEnv * env, jobject, jfloatArray textureMat)
  {
//    LOG("RenderTextrue JNI====================");
    GLint saved_viewport[4];
	glGetIntegerv(GL_VIEWPORT, saved_viewport);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, mediaTextureID);

    // OpenGL state changes:
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Unity does not unbind these buffers before we are called, so we have to do so.
    // This shouldn't have an effect on other rendering solutions as long as
    // the programmer is binding these buffers properly.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if(_glVersion > 2)
      glBindVertexArray(0);

    glUseProgram(shaderProgramID);

    glVertexAttribPointer(vertexHandle, 3, GL_FLOAT, GL_FALSE, 0,
                          (const GLvoid*) &orthoQuadVertices[0]);

    //视频格式转换
     switch(videoFormat)
     {
        case Common :
               glVertexAttribPointer(textureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0,
                                      (const GLvoid*) &orthoQuadTexCoords[0]);
                break;
        case LeftRigth :
                glVertexAttribPointer(textureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0,
                                                      (const GLvoid*) &letfRightTexCoords[0]);
                break;
        case UpDown :
                glVertexAttribPointer(textureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0,
                                                      (const GLvoid*) &upDownTexCoords[0]);
                break;
        default:
            glVertexAttribPointer(textureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0,
                                                  (const GLvoid*) &orthoQuadTexCoords[0]);
     }

    glViewport(0, 0, Width, Height);

    glEnableVertexAttribArray(vertexHandle);
    glEnableVertexAttribArray(textureCoordHandle);

    float *textureMatArray = env->GetFloatArrayElements(textureMat, 0);

    glUniformMatrix4fv(mvpMatrixHandle, 1, GL_FALSE,
                       (GLfloat*) &orthoProjMatrix[0]);
    glUniformMatrix4fv(textureMatrixHandle, 1, GL_FALSE,
                       (GLfloat*) textureMatArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE,
                   (const GLvoid*) &orthoQuadIndices[0]);

    env->ReleaseFloatArrayElements(textureMat, textureMatArray, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glUseProgram(0);  //测试

    shaderHelp::checkGlError("VuforiaMedia copyTexture");

    glViewport(saved_viewport[0], saved_viewport[1], saved_viewport[2], saved_viewport[3]);
  }

  JNIEXPORT void JNICALL Java_com_HuaMedia_HuaMediaJNI_native_1CommonFormat
    (JNIEnv *, jobject)
    {

        videoFormat = Common;
    }


  JNIEXPORT void JNICALL Java_com_HuaMedia_HuaMediaJNI_native_1LeftRightFormat
    (JNIEnv *, jobject)
    {
        LOG("LeftRightFormat  JNI ===================");
        videoFormat = LeftRigth;
    }


  JNIEXPORT void JNICALL Java_com_HuaMedia_HuaMediaJNI_native_1UpDownFormat
    (JNIEnv *, jobject)
    {
        videoFormat = UpDown;
    }

JNIEXPORT void JNICALL Java_com_HuaMedia_HuaMediaJNI_Native_1Release
  (JNIEnv *, jobject)
  {

  }