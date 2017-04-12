

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libHuaMediaJNI
LOCAL_ARM_MODE  := arm
LOCAL_SRC_FILES := HuaMediaJNI.cpp shaderHelp.cpp Utils.cpp
LOCAL_LDLIBS    := -llog -lGLESv3
LOCAL_SHARED_LIBRARIES +=\
        libcutils

include $(BUILD_SHARED_LIBRARY)
