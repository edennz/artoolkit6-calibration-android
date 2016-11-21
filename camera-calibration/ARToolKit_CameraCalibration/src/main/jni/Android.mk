#--------------------------------------------------------------------------
#
#  calib_camera
#  ARToolKit for Android
#
#  This file is part of ARToolKit.
#
#  ARToolKit is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  ARToolKit is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with ARToolKit.  If not, see <http://www.gnu.org/licenses/>.
#
#  As a special exception, the copyright holders of this library give you
#  permission to link this library with independent modules to produce an
#  executable, regardless of the license terms of these independent modules, and to
#  copy and distribute the resulting executable under terms of your choice,
#  provided that you also meet, for each linked independent module, the terms and
#  conditions of the license of that module. An independent module is a module
#  which is neither derived from nor based on this library. If you modify this
#  library, you may extend this exception to your version of the library, but you
#  are not obligated to do so. If you do not wish to do so, delete this exception
#  statement from your version.
#
#  Copyright 2015-2016 Daqri, LLC.
#  Copyright 2011-2015 ARToolworks, Inc.
#
#  Author(s): Philip Lamb
#
#--------------------------------------------------------------------------
#Returns the current directory that contains this Android.mk file to custom make variable
MY_LOCAL_PATH := $(call my-dir)
#Set the reserved make path variable
LOCAL_PATH := $(MY_LOCAL_PATH)

# Pull ARToolKit into the build
include $(CLEAR_VARS)
#ARToolKit home directory
ARTOOLKIT_DIR := $(MY_LOCAL_PATH)/../../../../../../android
ARTOOLKIT_LIBDIR := $(call host-path,$(ARTOOLKIT_DIR)/obj/local/$(TARGET_ARCH_ABI))

$(info $$MY_LOCAL_PATH: "$(MY_LOCAL_PATH)")
$(info $$ARTOOLKIT_DIR: "$(ARTOOLKIT_DIR)")
$(info $$ARTOOLKIT_LIBDIR: "$(ARTOOLKIT_LIBDIR)")

define add_artoolkit_module
    include $(CLEAR_VARS)
    LOCAL_MODULE:=$1
    LOCAL_SRC_FILES:=lib$1.a
    include $(PREBUILT_STATIC_LIBRARY)
endef

$(info Step1: Add AR prebuilt static libs)
ARTOOLKIT_LIBS := ar aricp util
LOCAL_PATH := $(ARTOOLKIT_LIBDIR)
$(foreach module,$(ARTOOLKIT_LIBS),$(eval $(call add_artoolkit_module,$(module))))

# Resets LOCAL_PATH to "[AS project jni dir]"
LOCAL_PATH := $(MY_LOCAL_PATH)

# Pull CURL into the build
#$(info Step2: Add curl prebuilt static lib)
CURL_DIR := $(ARTOOLKIT_DIR)/jni/curl
CURL_LIBDIR := $(call host-path,$(CURL_DIR)/libs/$(TARGET_ARCH_ABI))
define add_curl_module
    include $(CLEAR_VARS)
    LOCAL_MODULE:=$1
    LOCAL_SRC_FILES:=lib$1.a
    include $(PREBUILT_STATIC_LIBRARY)
endef
CURL_LIBS := curl
LOCAL_PATH := $(CURL_LIBDIR)
$(foreach module,$(CURL_LIBS),$(eval $(call add_curl_module,$(module))))

include $(CLEAR_VARS)

$(info Step4: build calib_camera and create shared libs)
# ARToolKit libs use lots of floating point, so don't compile in thumb mode.
#LOCAL_ARM_MODE := arm

LOCAL_PATH := $(MY_LOCAL_PATH)
LOCAL_MODULE := calibration_upload_native
LOCAL_SRC_FILES := calib_camera.cpp fileUploader.cpp

# Silence the warning caused by empty struct lconv { } in /usr/include/locale.h.
LOCAL_CPPFLAGS += -Wno-extern-c-compat

# Make sure DEBUG is defined for debug builds. (NDK already defines NDEBUG for release builds.)
ifeq ($(APP_OPTIM),debug)
    LOCAL_CPPFLAGS += -DDEBUG
    LOCAL_CFLAGS += -DDEBUG
endif

LOCAL_C_INCLUDES += $(ARTOOLKIT_DIR)/../include/android $(ARTOOLKIT_DIR)/../include
LOCAL_C_INCLUDES += $(CURL_DIR)/include
LOCAL_LDLIBS += -llog -lGLESv1_CM -lz
LOCAL_WHOLE_STATIC_LIBRARIES += ar
#LOCAL_STATIC_LIBRARIES += opencv_calib3d opencv_features2d opencv_imgproc opencv_flann opencv_core eden jpeg argsub_es armulti arosg aricp util cpufeatures
LOCAL_STATIC_LIBRARIES += aricp util cpufeatures
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_STATIC_LIBRARIES += tbb
endif
ifeq ($(TARGET_ARCH_ABI),x86)
    LOCAL_STATIC_LIBRARIES += tbb
endif
LOCAL_STATIC_LIBRARIES += curl

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)
