#include <jni.h>
#include <string>
#include <thread>
#include <android/native_window_jni.h>
#include "platform_service.h"
#include "data_dumper.h"
#include "glog/logging.h"

DataDumper dataDumper;

PerfMonitor imu_perf, acc_perf, gyr_perf;
int imu_fps, acc_fps, gyr_fps;

void rgbCallback(rgb_msg &msg) {
//    TimeLagMeasurer timer;
//    LOGI("rgb image latency %f", timer.getCurrentTimeSecond() - msg.ts);
    dataDumper.dumpRgbImage(msg);
    updatePreviewMat(msg.yMat, true);
    takeRgbSingleShot(msg);
}

void imuCallback(imu_msg &msg) {
//    TimeLagMeasurer timer;
//    LOGI("imu latency %f", timer.getCurrentTimeSecond() - msg.ts);
    dataDumper.dumpImuData(msg);
    imu_perf.update(imu_fps);
}

void accCallback(acc_msg &msg) {
//    TimeLagMeasurer timer;
    dataDumper.dumpAccData(msg);
//    LOG_EVERY_N(INFO, 10) << "acc dump cost " << timer.lagFromStartSecond()*1e3;
    acc_perf.update(acc_fps);
}

void gyrCallback(gyr_msg &msg) {
//    TimeLagMeasurer timer;
    dataDumper.dumpGyroData(msg);
//    LOG_EVERY_N(INFO, 10) << "gyr callback function cost " << timer.lagFromStartSecond()*1e3;
    gyr_perf.update(gyr_fps);
}

void featureCallback(double ts, std::vector<std::pair<size_t, Eigen::VectorXf>> &features) {
}

PlatformService platformService(imuCallback, accCallback, gyrCallback, rgbCallback,
                                featureCallback);


extern "C"
JNIEXPORT void JNICALL
Java_com_zhaoqun_vidata_MainActivity_startJNI(JNIEnv *env, jobject thiz) {
    platformService.start();

}
extern "C"
JNIEXPORT void JNICALL
Java_com_zhaoqun_vidata_MainActivity_stopJNI(JNIEnv *env, jobject thiz) {
    platformService.stop();
}
/*extern "C"
JNIEXPORT void JNICALL
Java_com_zhaoqun_vidata_MainActivity_sendPathToJNI(JNIEnv *env, jobject thiz, jstring app_storage) {
    const char *cstr = env->GetStringUTFChars(app_storage, nullptr);
    app_internal_storage = std::string(cstr);
    env->ReleaseStringUTFChars(app_storage, cstr);
    LOGW("app internal storage: %s", app_internal_storage.c_str());
}*/
extern "C"
JNIEXPORT void JNICALL
Java_com_zhaoqun_vidata_MainActivity_sendSurfaceToJNI(JNIEnv *env, jobject thiz, jobject cam_sf) {
    preview_native_window = ANativeWindow_fromSurface(env, cam_sf);
    ANativeWindow_acquire(preview_native_window);

    int image_w, image_h;
    std::string file_name = "sdcard/VIdata/config.yaml";
    cv::FileStorage fs(file_name, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        LOG(ERROR) << "cv::FileStorage open file failed -> " << file_name;
        return;
    }
    image_w = fs["rgb_width"];
    image_h = fs["rgb_height"];
    fs.release();
    // ANativeWindow_setBuffersGeometry(preview_native_window, image_w, image_h,
    //                                  WINDOW_FORMAT_RGBA_8888);
    /// if rotate
    ANativeWindow_setBuffersGeometry(preview_native_window, image_h, image_w,
                                     WINDOW_FORMAT_RGBA_8888);
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_zhaoqun_vidata_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string fps_string = "";
    if (platformService.imuPublisher_ != nullptr) {
        fps_string =
                "imu " + std::to_string(imu_fps) +
                " hz" + "\n" +
                "acc " + std::to_string(acc_fps) +
                " hz" + "\n" +
                "gyr " + std::to_string(gyr_fps) +
                " hz" + "\n";
    }
    if (platformService.camPublisher_ != nullptr) {
        fps_string += "rgb " + std::to_string(platformService.camPublisher_->fps_) + " hz" + "\n";
    }

    return env->NewStringUTF(fps_string.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zhaoqun_vidata_MainActivity_startDumpJNI(JNIEnv *env, jobject thiz) {
     dataDumper.start();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zhaoqun_vidata_MainActivity_stopDumpJNI(JNIEnv *env, jobject thiz) {
     dataDumper.stop();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zhaoqun_vidata_MainActivity_takeRgbSingleShotJNI(JNIEnv *env, jobject thiz) {
    rgb_single_shot = true;
}