/*
* 时间：2017-1-18
* 创造者：ChenXingHua
*/

//顶点shader
static const char* VertexShader = " \
  \
attribute vec4 vertexPosition; \
attribute vec2 vertexTexCoord; \
 \
varying vec2 texCoord; \
 \
uniform mat4 modelViewProjectionMatrix; \
uniform mat4 textureMatrix; \
 \
void main() \
{ \
   gl_Position = modelViewProjectionMatrix * vertexPosition; \
   vec4 temp = vec4(vertexTexCoord.x, 1.0 - vertexTexCoord.y, 1, 1); \
   texCoord = (textureMatrix * temp).xy; \
} \
";

//片段shader
static const char* FragmentShader = " \
 \
#extension GL_OES_EGL_image_external : require \n \
 \
precision mediump float; \
 \
varying vec2 texCoord; \
 \
uniform samplerExternalOES texSampler2D; \
 \
void main() \
{ \
   gl_FragColor = texture2D(texSampler2D, texCoord); \
} \
";
class shaderHelp {

    public:
        // 打印GL错误
        static void checkGlError(const char* operation);
        //初始化shader
        static unsigned int initShader(unsigned int shaderType,const char* source);
        //创建shader句柄
        static unsigned int createProgramFromBuffer(const char* vertexShaderBuffer,const char* fragmentShaderBuffer);
};
