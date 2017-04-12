/*
* 时间：2017-1-18
* 创造者：ChenXingHua
* 脚本：shader
* */
#include<shaderHelp.h>

#include <math.h>
#include <stdlib.h>

#include<Utils.h>

#define USE_OPENGL_ES_2_0 1

#ifdef USE_OPENGL_ES_1_1
#include <GLES/gl.h>
#include <GLES/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

void
shaderHelp::checkGlError(const char* operation)
{
    for (GLint error = glGetError(); error; error = glGetError())
        LOG("after %s() glError (0x%x)", operation, error);
}

unsigned int
shaderHelp::initShader(unsigned int shaderType, const char* source)
{
#ifdef USE_OPENGL_ES_2_0
    GLuint shader = glCreateShader((GLenum)shaderType);
    if (shader)
    {
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char* buf = (char*) malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOG("Could not compile shader %d: %s",
                        shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
#else
    return 0;
#endif
}


unsigned int
shaderHelp::createProgramFromBuffer(const char* vertexShaderBuffer,
                                     const char* fragmentShaderBuffer)
{
#ifdef USE_OPENGL_ES_2_0

    GLuint vertexShader = initShader(GL_VERTEX_SHADER, vertexShaderBuffer);
    if (!vertexShader)
        return 0;

    GLuint fragmentShader = initShader(GL_FRAGMENT_SHADER,
                                        fragmentShaderBuffer);
    if (!fragmentShader)
        return 0;

    GLuint program = glCreateProgram();
    if (program)
    {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");

        glAttachShader(program, fragmentShader);
        checkGlError("glAttachShader");

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*) malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOG("Could not link program: %s", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
#else
    return 0;
#endif
}