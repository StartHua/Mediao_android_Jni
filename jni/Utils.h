/*
* 时间：2017-1-18
* 创造者：ChenXingHua
*/
#include <android/log.h>

//注意要使用JAVA的啊打印1.必须加入头文件<android/log.h>
//2.Android.mk加入LOCAL_SHARED_LIBRARIES +=\ libcutils
#define LOG_TAG    "Xinghua"
#define LOG(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class Utils{

};