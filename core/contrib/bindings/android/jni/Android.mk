LOCAL_PATH := $(call my-dir)
 
include $(CLEAR_VARS)
LOCAL_MODULE    := libzmq 
LOCAL_SRC_FILES := libzmq.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libumundo 
LOCAL_SRC_FILES := libumundo.a
include $(PREBUILT_STATIC_LIBRARY)

 include $(CLEAR_VARS)
LOCAL_MODULE    := libumundoSwig 
LOCAL_SRC_FILES := libumundoSwig.so
include $(PREBUILT_SHARED_LIBRARY)
