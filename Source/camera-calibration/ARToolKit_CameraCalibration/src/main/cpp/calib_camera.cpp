/*
 *  calib_camera.cpp
 *  ARToolKit for Android
 *
 *  This file is part of ARToolKit.
 *
 *  ARToolKit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ARToolKit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ARToolKit.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  As a special exception, the copyright holders of this library give you
 *  permission to link this library with independent modules to produce an
 *  executable, regardless of the license terms of these independent modules, and to
 *  copy and distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the terms and
 *  conditions of the license of that module. An independent module is a module
 *  which is neither derived from nor based on this library. If you modify this
 *  library, you may extend this exception to your version of the library, but you
 *  are not obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  Copyright 2015-2016 Daqri LLC. All Rights Reserved.
 *  Copyright 2012-2015 ARToolworks, Inc. All Rights Reserved.
 *
 *  Author(s): Philip Lamb, Thorsten Bux
 */


/*
 * Design notes:
 *
 * New frames arrive on the main thread via nativeVideoFrame, and a copy of
 * both planes is made.
 *
 * If processing of frames (looking for the chessboard) is not needed, then a marker is
 * set that a new frame has arrived. Upload of this frame will be done on the OpenGL thread.
 * If processing of frames is active, then on the first frame, the cornerFinder thread
 * will be waiting, and the luma channel of the frame will be copied into the thread's data
 * and the thread started. On subsequent incoming frames, a check will be done for any
 * previous results from the cornerFinderThread first. If new results are available, the
 * luma for the processed frame will be copied again and a flag set that it should be
 * displayed, and the corner locations will be copied out.
 *
 * On the OpenGL thread, background frame upload is done,
 * then (if processing active) the drawing of the luma of the most recent frame processed,
 * and corners found.
 *
 * User interaction with this process comes via touches on the surface (delivered via
 * nativeHandleTouchAtLocation). Touches are processed on the main thread. If a touch
 * has been found, the most recent results are copied, and if 10 results have been copied,
 * then calibration proceeds, followed by saving of the calibration parameters. Finally,
 * an index file is written for processing by the upload thread.
 *
 * The upload thread pushes form data and the calibration file to the server.
 *
 */
// ============================================================================
//	Includes
// ============================================================================

#include "calib_camera.h"

#include <pthread.h>

#include "fileUploader.h"
#include "android_os_build_codes.h"

//#include <openssl/md5.h>
// Rather than including full OpenSSL header tree, just provide prototype for MD5().
// Usage is here: http://www.openssl.org/docs/crypto/md5.html.
#define MD5_DIGEST_LENGTH 16

// ============================================================================
//	Constants
// ============================================================================

// Logging macros
#define  LOG_TAG    "calibrationUploadNative"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define      SAVE_FILENAME                 "camera_para.dat"

// Data upload.
#define QUEUE_DIR "queue"
#define COPY_DIR "calibs"

#define QUEUE_INDEX_FILE_EXTENSION "upload"
//#define UPLOAD_POST_URL "https://omega.artoolworks.com/app/calib_camera/upload.php"
#define UPLOAD_POST_URL "http://192.168.1.42:8080/app/calib_camera/upload.php"
#define UPLOAD_STATUS_HIDE_AFTER_SECONDS 9.0f

// ============================================================================
//	Global variables
// ============================================================================

// Image acquisition.
static int videoWidth = 0;                                          ///< Width of the video frame in pixels.
static int videoHeight = 0;                                         ///< Height of the video frame in pixels.
static int gCameraIndex = 0;
static bool gCameraIsFrontFacing = false;
char gHashedToken[32];

JavaVM *jvm = NULL;

//
// Data upload.
//
static FILE_UPLOAD_HANDLE_t *fileUploadHandle = NULL;


// ============================================================================
//	Android NDK function signatures
// ============================================================================

jobject objectCameraCalibActivity;

// Utility preprocessor directive so only one change needed if Java class name changes
// _1 is the escape sequence for a '_' character.
void wireupJavaMethods(JNIEnv *pEnv, jobject pJobject);

void c_nativeSaveParam(float min, float average, int sizeX, float max, int sizeY, jsize camMatLen,
                       jsize distLen, const jdouble *distortionCoefficientsArray,
                       const jdouble *cameraMatrix);

#define JNIFUNCTION_NATIVE(sig) Java_org_artoolkit_ar6_calib_1camera_CameraCalibrationActivity_##sig

extern "C" {
JNIEXPORT void JNICALL JNIFUNCTION_NATIVE(nativeSaveParam(JNIEnv *env, jobject type,
        jdoubleArray cameraMatrix, jdoubleArray distortionCoefficientsArray_,int sizeX, int sizeY,
        float average, float min, float max));
JNIEXPORT bool JNICALL JNIFUNCTION_NATIVE(nativeSaveParamToARTK(JNIEnv *env, jobject type,
        jdoubleArray cameraMatrix, jdoubleArray distortionCoefficientsArray_,int sizeX, int sizeY,
        float average, float min, float max, jstring calibrationServerARTKUrl, jstring artkToken));
JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeInitialize(JNIEnv *env, jobject type,
                      jobject instanceOfAndroidContext, jstring calibrationServerUrl,jint cameraIndex, jboolean cameraIsFrontFacing, jstring token));
JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeStop(JNIEnv *env, jobject type));
};

// Save parameters file and index file with info about it, then signal thread that it's ready for upload.
void saveParam(const ARParam *param, ARdouble err_avg, ARdouble err_min, ARdouble err_max) {
    int i;
    #define SAVEPARAM_PATHNAME_LEN 80
    #define COPY_PARAM_PATHNAME_LEN PATH_MAX

    char indexPathname[SAVEPARAM_PATHNAME_LEN];
    char paramPathname[SAVEPARAM_PATHNAME_LEN];
    char destPathname[COPY_PARAM_PATHNAME_LEN];
    char indexUploadPathname[SAVEPARAM_PATHNAME_LEN];

    LOGD("Entering saveParam err_min: %lf, err_avg: %lf, err_max: %lf",err_min, err_avg, err_max);

    // Get the current time. It will be used for file IDs, plus a timestamp for the parameters file.
    time_t ourClock = time(NULL);
    if (ourClock == (time_t) -1) {
        LOGE("Error reading time and date.\n");
        return;
    }
    //struct tm *timeptr = localtime(&ourClock);
    struct tm *timeptr = gmtime(&ourClock);
    if (!timeptr) {
        LOGE("Error converting time and date to UTC.\n");
        return;
    }

    LOGD("About to enter fileUploaderCreateQueueDir fileUploadHandle: %p",fileUploadHandle);
    // Check for QUEUE_DIR and create if not already existing.
    if (!fileUploaderCreateDir(QUEUE_DIR)) {
        return;
    }

    // Save the parameter file.
    snprintf(paramPathname, SAVEPARAM_PATHNAME_LEN, "%s/camera_para.dat", QUEUE_DIR);
    if (arParamSave(paramPathname, 1, param) < 0) {

        LOGE("Error writing camera_para.dat file.\n");

    } else {
        LOGD("%s/camera_para.dat created in %s",QUEUE_DIR,paramPathname);
        char device_id[PROP_VALUE_MAX * 3 +
                       2]; // From <sys/system_properties.h>. 3 properties plus separators.
        char camera_index[12]; // 10 digits in INT32_MAX, plus sign, plus null.
        char camera_width[12]; // 10 digits in INT32_MAX, plus sign, plus null.
        char camera_height[12]; // 10 digits in INT32_MAX, plus sign, plus null.
        char focal_length[] = "0.000";

        //
        // Write an upload index file with the data for the server database entry.
        //

        bool goodWrite = true;

        // Open the file.
        snprintf(indexPathname, SAVEPARAM_PATHNAME_LEN, "%s/index", QUEUE_DIR);
        FILE *fp;
        if (!(fp = fopen(indexPathname, "wb"))) {
            LOGE("Error opening upload index file '%s'.\n", indexPathname);
            goodWrite = false;
        }

        // File name.
        if (goodWrite) fprintf(fp, "file,%s\n", paramPathname);

        // UTC date and time, in format "1999-12-31 23:59:59 UTC".
        if (goodWrite) {
            char timestamp[26 + 8] = "";
            if (!strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S %z", timeptr)) {
                LOGE("Error formatting time and date.\n");
                goodWrite = false;
            } else {
                fprintf(fp, "timestamp,%s\n", timestamp);
            }
        }

        // OS: name/arch/version.
        if (goodWrite) {
            char os[PROP_VALUE_MAX];
            fprintf(fp, "os_name,android\n");
            __system_property_get(ANDROID_OS_BUILD_CPU_ABI, os);
            fprintf(fp, "os_arch,%s\n", os);
            __system_property_get(ANDROID_OS_BUILD_VERSION_RELEASE, os);
            fprintf(fp, "os_version,%s\n", os);
        }

        // Handset ID, via <sys/system_properties.h>.
        if (goodWrite) {
            int len;
            len = __system_property_get(ANDROID_OS_BUILD_MANUFACTURER,
                                        device_id); // len = (int)strlen(device_id).
            device_id[len] = '/';
            len++;
            len += __system_property_get(ANDROID_OS_BUILD_MODEL, device_id + len);
            device_id[len] = '/';
            len++;
            __system_property_get(ANDROID_OS_BUILD_BOARD, device_id + len);
            fprintf(fp, "device_id,%s\n", device_id);
        }

        // Focal length in metres.
        // Not known at present, so just send 0.000.
        if (goodWrite) {
            fprintf(fp, "focal_length,%s\n", focal_length);
        }

        // Camera index.
        if (goodWrite) {
            snprintf(camera_index, 12, "%d", gCameraIndex);
            fprintf(fp, "camera_index,%s\n", camera_index);
        }

        // Front or rear facing.
        if (goodWrite) {
            char camera_face[6]; // "front" or "rear", plus null.
            snprintf(camera_face, 6, "%s", (gCameraIsFrontFacing ? "front" : "rear"));
            fprintf(fp, "camera_face,%s\n", camera_face);
        }

        // Camera dimensions.
        if (goodWrite) {
            snprintf(camera_width, 12, "%d", videoWidth);
            snprintf(camera_height, 12, "%d", videoHeight);
            fprintf(fp, "camera_width,%s\n", camera_width);
            fprintf(fp, "camera_height,%s\n", camera_height);
        }

        // Calibration error.
        if (goodWrite) {
            char err_min_ascii[12];
            char err_avg_ascii[12];
            char err_max_ascii[12];
            snprintf(err_min_ascii, 12, "%f", err_min);
            snprintf(err_avg_ascii, 12, "%f", err_avg);
            snprintf(err_max_ascii, 12, "%f", err_max);
            fprintf(fp, "err_min,%s\n", err_min_ascii);
            fprintf(fp, "err_avg,%s\n", err_avg_ascii);
            fprintf(fp, "err_max,%s\n", err_max_ascii);
        }

        // Write the token.
        if (goodWrite) {
            fprintf(fp, "ss,%s\n", gHashedToken);
        }

        // Done writing index file.
        fclose(fp);

        LOGD("camera_para.dat has been written");

        snprintf(destPathname, COPY_PARAM_PATHNAME_LEN, "%s/camera_para-",COPY_DIR);

        size_t len = strlen(destPathname);
        int i = 0;
        while (device_id[i] && (len + i + 2 < SAVEPARAM_PATHNAME_LEN)) {
            destPathname[len + i] = (device_id[i] == '/' || device_id[i] == '\\' ? '_' : device_id[i]);
            i++;
        }
        destPathname[len + i] = '\0';
        len = strlen(destPathname);
        snprintf(&destPathname[len], COPY_PARAM_PATHNAME_LEN - len, "-%s-%sx%s",camera_index,camera_width,camera_height);

        len = strlen(destPathname);
        if (strcmp(focal_length, "0.000") != 0) {
            snprintf(&destPathname[len], SAVEPARAM_PATHNAME_LEN - len, "-%s", focal_length);
            len = strlen(destPathname);
        }
        snprintf(&destPathname[len], SAVEPARAM_PATHNAME_LEN - len, ".dat");
        //snprintf(destPathname, COPY_PARAM_PATHNAME_LEN, "%s/%s_%06d-camera_para.dat",COPY_DIR,timestamp4Calib, ID);


        //Writing camera_para a second time to make it available to the user for sharing/download
        fileUploaderCreateDir(COPY_DIR);
        LOGD("Try to create %s for sharing", destPathname);
        if (arParamSave(destPathname, 1, param) < 0) {
            LOGE("Error writing copy of camera_para.dat file.\n");
        }


        if (goodWrite) {
            // Rename the file with QUEUE_INDEX_FILE_EXTENSION file extension so it's picked up in uploader.
            snprintf(indexUploadPathname, SAVEPARAM_PATHNAME_LEN, "%s.upload", indexPathname);
            if (rename(indexPathname, indexUploadPathname) < 0) {
                LOGE("Error renaming temporary file '%s'.\n", indexPathname);
                goodWrite = false;
            } else {
                // Kick off an upload handling cycle.
                LOGD("Calling fileUploaderTickle");
                fileUploaderTickle(fileUploadHandle);
            }
        }

        if (!goodWrite) {
            // Delete the index and param files.
            if (remove(indexPathname) < 0) {
                LOGE("Error removing temporary file '%s'.\n", indexPathname);
                ARLOGperror((const char *) NULL);
            }
            if (remove(paramPathname) < 0) {
                LOGE("Error removing temporary file '%s'.\n", paramPathname);
                ARLOGperror((const char *) NULL);
            }
        }
    }
}

void convParam(float dist[4], int xsize, int ysize, float fx,float fy,float x0,float y0, ARParam *param)
{
    LOGI("Called convParam with x: %d, y: %d",xsize,ysize);

    //Initialization of the intrinsics array. The values marked with -1 need to be replaces with the
    //values coming from distortionCoefficientsArray
    float intr[3][4] =
            {
                {-1.0f, 0.0f, -1.0f, 0.0f} ,   /*  initializers for row indexed by 0 */
                {0.0f, -1.0f, -1.0f, 0.0f} ,   /*  initializers for row indexed by 1 */
                {0.0f, 0.0f, 1.0f, 0.0f}   /*  initializers for row indexed by 2 */
            };

    intr[0][0] = fx;
    intr[0][2] = x0;
    intr[1][1] = fy;
    intr[1][2] = y0;

    for(int i=0; i < 3; i++){
        for (int z = 0; z < 4; z++){
            LOGD("intr[%d][%d]: %f",i,z,intr[i][z]);
        }
    }
    for(int i=0; i < 4; i++){
        LOGD("dist[%d]: %f",i,dist[i]);
    }

    double   s;
    int      i, j;

    param->dist_function_version = AR_DIST_FUNCTION_VERSION_MAX;
    param->xsize = xsize;
    param->ysize = ysize;

    param->dist_factor[0] = (ARdouble)dist[0];     /* k1  */
    param->dist_factor[1] = (ARdouble)dist[1];     /* k2  */
    param->dist_factor[2] = (ARdouble)dist[2];     /* p1  */
    param->dist_factor[3] = (ARdouble)dist[3];     /* p2  */
    param->dist_factor[4] = (ARdouble)intr[0][0];  /* fx  */
    param->dist_factor[5] = (ARdouble)intr[1][1];  /* fy  */
    param->dist_factor[6] = (ARdouble)intr[0][2];  /* x0  */
    param->dist_factor[7] = (ARdouble)intr[1][2];  /* y0  */
    param->dist_factor[8] = (ARdouble)1.0;         /* s   */

    for( j = 0; j < 3; j++ ) {
        for( i = 0; i < 4; i++ ) {
            param->mat[j][i] = (ARdouble)intr[j][i];
        }
    }

    s = getSizeFactor(param->dist_factor, xsize, ysize, param->dist_function_version);
    param->mat[0][0] /= s;
    param->mat[0][1] /= s;
    param->mat[1][0] /= s;
    param->mat[1][1] /= s;
    param->dist_factor[8] = s;
}

ARdouble getSizeFactor(ARdouble dist_factor[], int xsize, int ysize, int dist_function_version)
{
    ARdouble  ox, oy, ix, iy;
    ARdouble  olen, ilen;
    ARdouble  sf, sf1;

    sf = 100.0f;

    ox = 0.0f;
    oy = dist_factor[7];
    olen = dist_factor[6];
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = dist_factor[6] - ix;
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = xsize;
    oy = dist_factor[7];
    olen = xsize - dist_factor[6];
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = ix - dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = dist_factor[6];
    oy = 0.0;
    olen = dist_factor[7];
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = dist_factor[7] - iy;
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = dist_factor[6];
    oy = ysize;
    olen = ysize - dist_factor[7];
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = iy - dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }


    ox = 0.0f;
    oy = 0.0f;
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = dist_factor[6] - ix;
    olen = dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }
    ilen = dist_factor[7] - iy;
    olen = dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = xsize;
    oy = 0.0f;
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = ix - dist_factor[6];
    olen = xsize - dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }
    ilen = dist_factor[7] - iy;
    olen = dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = 0.0f;
    oy = ysize;
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = dist_factor[6] - ix;
    olen = dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }
    ilen = iy - dist_factor[7];
    olen = ysize - dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    ox = xsize;
    oy = ysize;
    arParamObserv2Ideal( dist_factor, ox, oy, &ix, &iy, dist_function_version );
    ilen = ix - dist_factor[6];
    olen = xsize - dist_factor[6];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }
    ilen = iy - dist_factor[7];
    olen = ysize - dist_factor[7];
    //ARLOG("Olen = %f, Ilen = %f, s = %f\n", olen, ilen, ilen / olen);
    if( ilen > 0.0f ) {
        sf1 = ilen / olen;
        if( sf1 < sf ) sf = sf1;
    }

    if( sf == 100.0f ) sf = 1.0f;

    return sf;
}

JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeInitialize(JNIEnv *env, jobject type,
        jobject instanceOfAndroidContext,jstring calibrationServerUrl, jint cameraIndex, jboolean cameraIsFrontFacing, jstring token)){

    //arLogLevel = AR_LOG_LEVEL_DEBUG;

    const char *calibServerUrl = env->GetStringUTFChars(calibrationServerUrl, 0);
    const char *hashedToken = env->GetStringUTFChars(token,0);
    LOGD("Entered nativeInitialize with instanceOfAndroidContext: %p and CalibServerUrl: %s",instanceOfAndroidContext,calibServerUrl);
    arUtilChangeToResourcesDirectory(AR_UTIL_RESOURCES_DIRECTORY_BEHAVIOR_USE_APP_CACHE_DIR, NULL, instanceOfAndroidContext);

    gCameraIndex = cameraIndex;
    gCameraIsFrontFacing = cameraIsFrontFacing;
    strcpy(gHashedToken,hashedToken);

    //Save instance to JVM
    env->GetJavaVM(&jvm);
    objectCameraCalibActivity = env->NewGlobalRef(instanceOfAndroidContext);

    fileUploadHandle = fileUploaderInit(QUEUE_DIR, QUEUE_INDEX_FILE_EXTENSION, calibServerUrl,
                                        UPLOAD_STATUS_HIDE_AFTER_SECONDS);

    LOGD("fileUploadHandle: %p",fileUploadHandle);
    if (!fileUploadHandle) return ((jboolean) false);

    env->ReleaseStringUTFChars(calibrationServerUrl, calibServerUrl);
    env->ReleaseStringUTFChars(token,hashedToken);
    return (jboolean) fileUploaderTickle(fileUploadHandle);
}

JNIEXPORT bool JNICALL JNIFUNCTION_NATIVE(nativeSaveParamToARTK(JNIEnv *env, jobject type,
        jdoubleArray cameraMatrix_, jdoubleArray distortionCoefficientsArray_, int sizeX, int sizeY,
        float average, float min, float max, jstring calibrationServerARTKUrl, jstring artkToken )) {

    jsize camMatLen = env->GetArrayLength(cameraMatrix_);
    jsize distLen = env->GetArrayLength(distortionCoefficientsArray_);

    jdouble *distortionCoefficientsArray = env->GetDoubleArrayElements(distortionCoefficientsArray_,
                                                                       NULL);
    jdouble *cameraMatrix = env->GetDoubleArrayElements(cameraMatrix_,
                                                        NULL);

    //save old settings
    char hashedToken[32];
    strcpy(hashedToken,gHashedToken);
    const char *artkHashedToken = env->GetStringUTFChars(artkToken,0);
    strcpy(gHashedToken,artkHashedToken);

    const char *calibServerARTKUrl = env->GetStringUTFChars(calibrationServerARTKUrl, 0);


    _FILE_UPLOAD_HANDLE* oldFileUploadHandle = fileUploadHandle;

    fileUploadHandle = fileUploaderInit(QUEUE_DIR, QUEUE_INDEX_FILE_EXTENSION, calibServerARTKUrl,
                                        UPLOAD_STATUS_HIDE_AFTER_SECONDS);

    c_nativeSaveParam(min,average,sizeX,max,sizeY,camMatLen,distLen,distortionCoefficientsArray,cameraMatrix);

    //restore old settings
    strcpy(gHashedToken,hashedToken);

    fileUploaderFinal(&fileUploadHandle);
    fileUploadHandle = oldFileUploadHandle;

    env->ReleaseStringUTFChars(calibrationServerARTKUrl, calibServerARTKUrl);
    env->ReleaseStringUTFChars(artkToken,artkHashedToken);
    env->ReleaseDoubleArrayElements(distortionCoefficientsArray_, distortionCoefficientsArray, JNI_ABORT);
    env->ReleaseDoubleArrayElements(cameraMatrix_, cameraMatrix, JNI_ABORT);

}


JNIEXPORT void c_nativeSaveParam(float min, float average, int sizeX, float max, int sizeY, jsize camMatLen,
                                 jsize distLen, const jdouble *distortionCoefficientsArray,
                                 const jdouble *cameraMatrix) {
    LOGI("1 Called nativeSaveParam with x: %d, y: %d  ", sizeX, sizeY);
    videoWidth = sizeX;
    videoHeight = sizeY;

    LOGI("and with cameraMatrix size: %d and values: ", camMatLen);
    for (int i=0;i < camMatLen ;i++) {
        LOGD("%lf ",cameraMatrix[i]);
    }

    ARParam param;

//    setting the camera calibration values in the right place in the matrix
    float fx = (float) cameraMatrix[0];  /* fx */
    float x0 = (float) cameraMatrix[2];  /* x0 */
    float fy = (float) cameraMatrix[4];  /* fy */
    float y0 = (float) cameraMatrix[5];  /* y0 */

    float dist[4] = {-1.0f,-1.0f,-1.0f,-1.0f};

    for(int i = 0; i < distLen; i++){
        dist[i] = (float) distortionCoefficientsArray[i];
        LOGD("dist[%d]: %f",i, (float) distortionCoefficientsArray[i]);
    }

    convParam(dist,sizeX,sizeY,fx,fy,x0,y0,&param);
    saveParam(&param,average,min,max);
}

void JNICALL JNIFUNCTION_NATIVE(nativeSaveParam(JNIEnv * env, jobject type,
        jdoubleArray cameraMatrix_,
        jdoubleArray distortionCoefficientsArray_, int sizeX, int sizeY, float average, float min, float max)) {
    jsize camMatLen = env->GetArrayLength(cameraMatrix_);
    jsize distLen = env->GetArrayLength(distortionCoefficientsArray_);

    jdouble *distortionCoefficientsArray = env->GetDoubleArrayElements(distortionCoefficientsArray_,
                                                                       NULL);
    jdouble *cameraMatrix = env->GetDoubleArrayElements(cameraMatrix_,
                                                                       NULL);

    c_nativeSaveParam(min, average, sizeX, max, sizeY, camMatLen, distLen,
                      distortionCoefficientsArray, cameraMatrix);

    env->ReleaseDoubleArrayElements(distortionCoefficientsArray_, distortionCoefficientsArray, JNI_ABORT);
    env->ReleaseDoubleArrayElements(cameraMatrix_, cameraMatrix, JNI_ABORT);
}

JNIEXPORT jboolean JNICALL JNIFUNCTION_NATIVE(nativeStop(JNIEnv *env, jobject type)){
    fileUploaderFinal(&fileUploadHandle);
    env->DeleteGlobalRef(objectCameraCalibActivity);
    return (jboolean) true;
}