#cmake_minimum_required(VERSION 3.4.1)
#
#set(CMAKE_VERBOSE_MAKEFILE on)
#
#
## set binary output folder to Android assets folder
##set(EXECUTABLE_OUTPUT_PATH      "${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets/${ANDROID_ABI}")
## android 特定变量。。。使用LIBRARY_OUTPUT_PATH无效
##set(CMAKE_LIBRARY_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_SOURCE_DIR}/src/main/assets/${ANDROID_ABI}")
#
#set(ijkffmpeg_base_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../android/contrib/build)
#
#if(${ANDROID_ABI} STREQUAL armeabi)
#    set(ijkffmpeg_target_dir ${ijkffmpeg_base_dir}/ffmpeg-armv5/output)
#elseif(${ANDROID_ABI} STREQUAL armeabi-v7a)
#    set(ijkffmpeg_target_dir ${ijkffmpeg_base_dir}/ffmpeg-armv7a/output)
#elseif(${ANDROID_ABI} STREQUAL arm64-v8a)
#    set(ijkffmpeg_target_dir ${ijkffmpeg_base_dir}/ffmpeg-arm64/output)
#elseif(${ANDROID_ABI} STREQUAL x86)
#    set(ijkffmpeg_target_dir ${ijkffmpeg_base_dir}/ffmpeg-x86/output)
#elseif(${ANDROID_ABI} STREQUAL x86_64)
#    set(ijkffmpeg_target_dir ${ijkffmpeg_base_dir}/ffmpeg-x86_64/output)
#endif()
#
#set(ijkffmpeg_target_inc_dir ${ijkffmpeg_target_dir}/include)
#
#set(ijkffmpeg_target_lib_path ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI})
#
#add_subdirectory(${CMAKE_SOURCE_DIR}/src/main/cpp/android-ndk-prof)
#add_subdirectory(${CMAKE_SOURCE_DIR}/src/main/cpp/ijkmedia)



# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
#ComputeSumSquareError
#ADD_DEFINITIONS(-DHAVE_JPEG)

# modified by dj add c++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")

endif()


set(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")

set(BASE_JNI_DIR
        "${CMAKE_SOURCE_DIR}/src/main/cpp"
        )

set(
        third-libs
        ${CMAKE_SOURCE_DIR}/../../contrib/build/ffmpeg-armv7a/output/libijkffmpeg.so
)

cmake_minimum_required(VERSION 3.4.1)
INCLUDE_DIRECTORIES(
        ${CMAKE_SOURCE_DIR}/../../contrib/build/ffmpeg-armv7a/output/include
        ${BASE_JNI_DIR}/android-ndk-prof
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/android/media
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/android/os
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/java/nio
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/java/util
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/tv/danmaku/ijk/media/player
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/tv/danmaku/ijk/media/player/misc
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4au/class/android/media
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4au/class/java/nio
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/android
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/android/pipeline
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/pipeline
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/dummy
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ffmpeg
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2
        ${BASE_JNI_DIR}/ijkmedia/ijksdl
        ${BASE_JNI_DIR}/ijkmedia
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/include
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundStretch
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouchDLL
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/include
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/util
)

set( SRC_FILES
        ${BASE_JNI_DIR}/android-ndk-prof/prof.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/android/media/AudioTrack.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/android/media/MediaCodec.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/android/media/MediaFormat.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/android/media/PlaybackParams.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/android/os/Build.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/android/os/Bundle.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/java/nio/Buffer.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/java/nio/ByteBuffer.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/java/util/ArrayList.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/tv/danmaku/ijk/media/player/IjkMediaPlayer.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/tv/danmaku/ijk/media/player/misc/IAndroidIO.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/class/tv/danmaku/ijk/media/player/misc/IMediaDataSource.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/j4a_allclasses.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4a/j4a_base.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4au/class/android/media/AudioTrack.util.c
        ${BASE_JNI_DIR}/ijkmedia/ijkj4a/j4au/class/java/nio/ByteBuffer.util.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/android/ffmpeg_api_jni.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/android/ijkplayer_android.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/android/ijkplayer_jni.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/android/pipeline/ffpipeline_android.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/android/pipeline/ffpipenode_android_mediacodec_vdec.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ff_cmdutils.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ff_ffpipeline.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ff_ffpipenode.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ff_ffplay.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/allformats.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkasync.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkio.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkioandroidio.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkioapplication.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkiocache.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkioffio.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkiomanager.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkioprotocol.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkiourlhook.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijklivehook.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijklongurl.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkmediadatasource.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijksegment.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavformat/ijkurlhook.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavutil/ijkdict.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavutil/ijkfifo.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavutil/ijkthreadpool.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavutil/ijktree.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavutil/ijkutils.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkmeta.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkplayer.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/pipeline/ffpipeline_ffplay.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/pipeline/ffpipenode_ffplay_vdec.c
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/dj_record.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/android_audiotrack.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/android_nativewindow.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_android_jni.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_aout_android_audiotrack.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_aout_android_opensles.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_codec_android_mediacodec.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_codec_android_mediacodec_dummy.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_codec_android_mediacodec_internal.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_codec_android_mediacodec_java.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_codec_android_mediadef.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_codec_android_mediaformat.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_codec_android_mediaformat_java.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_vout_android_nativewindow.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_vout_android_surface.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/android/ijksdl_vout_overlay_android_mediacodec.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/dummy/ijksdl_vout_dummy.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ffmpeg/abi_all/image_convert.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ffmpeg/ijksdl_vout_overlay_ffmpeg.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/color.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/common.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/fsh/rgb.fsh.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/fsh/yuv420p.fsh.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/fsh/yuv420sp.fsh.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/fsh/yuv444p10le.fsh.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/renderer.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/renderer_rgb.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/renderer_yuv420p.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/renderer_yuv420sp.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/renderer_yuv444p10le.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/shader.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/gles2/vsh/mvp.vsh.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_aout.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_audio.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_egl.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_error.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_extra_log.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_mutex.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_stdinc.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_thread.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_timer.c
        ${BASE_JNI_DIR}/ijkmedia/ijksdl/ijksdl_vout.c
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/util/cpuid.c
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/compare.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/compare_common.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/compare_neon.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/compare_neon64.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/compare_posix.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/compare_win.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/convert.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/convert_argb.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/convert_from.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/convert_from_argb.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/convert_jpeg.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/convert_to_argb.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/convert_to_i420.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/cpu_id.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/format_conversion.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/mjpeg_decoder.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/mjpeg_validate.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/planar_functions.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/rotate.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/rotate_argb.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/rotate_mips.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/rotate_neon.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/rotate_neon64.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/row_any.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/row_common.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/row_mips.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/row_neon.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/row_neon64.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/row_posix.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/row_win.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/scale.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/scale_argb.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/scale_common.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/scale_mips.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/scale_neon.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/scale_neon64.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/scale_posix.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/scale_win.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/source/video_common.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/util/psnr.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkyuv/util/ssim.cc
        ${BASE_JNI_DIR}/ijkmedia/ijkplayer/ijkavutil/ijkstl.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/ijksoundtouch_wrap.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/Android-lib/jni/soundtouch-jni.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundStretch/RunParameters.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundStretch/WavFile.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/AAFilter.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/BPMDetect.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/cpu_detect_x86.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/FIFOSampleBuffer.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/FIRFilter.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/InterpolateCubic.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/InterpolateLinear.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/InterpolateShannon.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/mmx_optimized.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/PeakFinder.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/RateTransposer.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/SoundTouch.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/sse_optimized.cpp
        ${BASE_JNI_DIR}/ijkmedia/ijksoundtouch/source/SoundTouch/TDStretch.cpp
        )

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library( # Sets the name of the library.
        ijkplayer
        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${SRC_FILES} )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log
        )
find_library( android-lib android)
find_library( EGL-lib EGL)
find_library( GLESv2-lib GLESv2)
find_library( GLESv1_CM-lib GLESv1_CM)
find_library( OpenSLES-lib OpenSLES)
find_library( dl-lib dl)
find_library( z-lib z)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        ijkplayer
        # modified by dj    to make sure AndroidBitmap_lockPixels  can be linked in
        jnigraphics
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${android-lib}
        ${EGL-lib}
        ${GLESv2-lib}
        ${GLESv1_CM-lib}
        ${OpenSLES-lib}
        ${dl-lib}
        ${z-lib}
        ${third-libs}
        )