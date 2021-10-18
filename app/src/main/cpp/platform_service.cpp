//
// Created by zhongzhaoqun on 2021/4/30.
//

//#include <opencv2/opencv.hpp>
#include "platform_service.h"
#include "glog/logging.h"

void PlatformService::start() {
    std::string file_name = "sdcard/VIdata/config.yaml";
    cv::FileStorage fs(file_name, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        LOG(ERROR) << "cv::FileStorage open file failed -> " << file_name;
    }

    record_rgb = static_cast<int>(fs["record_rgb"]);
    fs.release();
    if (record_rgb) {
        camPublisher_ = std::make_unique<CamPublisher>(rgb_Callback_,nullptr);
        camPublisher_->start();
        previewer_.start();
    }
    imuPublisher_ = std::make_unique<ImuPublisher>(imu_callback_, acc_callback_, gyro_callback_);
    imuPublisher_->start();
    //shared_ptr_valid_ = true;
}

void PlatformService::stop() {
    if (record_rgb) {
        camPublisher_->stop();
        previewer_.stop();
    }
    imuPublisher_->stop();
}


