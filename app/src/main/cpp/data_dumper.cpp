#include <dirent.h>
#include <opencv2/imgcodecs.hpp>
#include "data_dumper.h"
#include <fstream>
#include <thread>
#include <iostream>
#include "glog/logging.h"

void DataDumper::dumpRgbImage(rgb_msg & image) {
    if (!dump_open_ || image.yMat.empty())
        return;
    std::ofstream fs(dump_path_ + "rgb_images/timeStamp" + image_ts_file_format_, std::ios::app);
    fs << image.ts << std::endl;
    fs.close();

    std::string folder = dump_path_ + "rgb_images/data/";
    std::string filename = folder + std::to_string(image.ts) + ".png";
    cv::imwrite(filename, image.yMat); //TODO: make this thread function
}


void DataDumper::dumpAccData(acc_msg & accMsg) {
    if (!dump_open_)
        return;
/*  std::ofstream accf(dump_path_ + "acc" + imu_file_format_, std::ios::app);
    accf << accMsg.ts << "," << accMsg.ax << "," <<accMsg.ay << "," <<accMsg.az << std::endl;
    accf.close();*/
    pthread_mutex_lock(&acc_mtx_);
    acc_queue_.push(accMsg);
    pthread_mutex_unlock(&acc_mtx_);
}

void DataDumper::dumpGyroData(gyr_msg &gyroMsg) {
    if (!dump_open_)
        return;
/*    std::ofstream gyrf(dump_path_ + "gyr" + imu_file_format_, std::ios::app);
    gyrf << gyroMsg.ts << "," << gyroMsg.rx << "," <<gyroMsg.ry << "," <<gyroMsg.rz << std::endl;
    gyrf.close();*/
    pthread_mutex_lock(&gyr_mtx_);
    gyr_queue_.push(gyroMsg);
    pthread_mutex_unlock(&gyr_mtx_);
}

void DataDumper::dumpImuData(imu_msg & imuMsg) {
    if (!dump_open_)
        return;
/*    std::ofstream imuf(dump_path_ + "imu" + imu_file_format_, std::ios::app);
    imuf << imuMsg.ts << "," << imuMsg.acc_part.ax << "," << imuMsg.acc_part.ay << ","
         << imuMsg.acc_part.az << ","
         << imuMsg.gyro_part.rx << "," << imuMsg.gyro_part.ry << "," << imuMsg.gyro_part.rz
         << std::endl;
    imuf.close();*/
    pthread_mutex_lock(&imu_mtx_);
    imu_queue_.push(imuMsg);
    pthread_mutex_unlock(&imu_mtx_);
}

void* imuDumpThreadRunner(void *ptr) {
    DataDumper* classptr = (DataDumper*)ptr;
    classptr->imuDumpThreadFunction();
    return nullptr;
}

void DataDumper::start() {
//    dump_path_ = app_internal_storage + "dump/";
    dump_path_ = "/sdcard/VIdata/dump/";
    //clear last dump
    DIR *dir = opendir(dump_path_.c_str());
    if (dir) {
        LOG(WARNING) << "dump path exists.";
        std::string cmd = "rm -r " + dump_path_;
        system(cmd.c_str());
    }
    std::string cmd = "mkdir -p ";
    system((cmd + dump_path_).c_str());
    system((cmd + dump_path_ + "rgb_images/").c_str());
    system((cmd + dump_path_ + "rgb_images/data/").c_str());

    cv::FileStorage fs("sdcard/VIdata/config.yaml", cv::FileStorage::READ);
    imu_file_format_ = static_cast<std::string>(fs["imu_file_format"]);
    image_ts_file_format_ = static_cast<std::string>(fs["ts_file_format"]);
    fs.release();

    dump_open_ = true;
    pthread_create(&main_th_, nullptr, imuDumpThreadRunner, this);
}

void DataDumper::stop() {
    dump_open_ = false;
    pthread_join(main_th_, nullptr);
}


void DataDumper::imuDumpThreadFunction() {
    while (dump_open_) {
        useconds_t thread_sleep_time = static_cast<useconds_t>(100);
        usleep(thread_sleep_time);

        std::queue<acc_msg> acc_buf; //here is empty
        pthread_mutex_lock(&acc_mtx_);
        if (!acc_queue_.empty()) {
            std::swap(acc_queue_, acc_buf); //here is filled with accumulated msgs, while acc_queue_ is emptied.
        }
        pthread_mutex_unlock(&acc_mtx_);

        std::queue<gyr_msg> gyr_buf;
        pthread_mutex_lock(&gyr_mtx_);
        if (!gyr_queue_.empty()) {
            std::swap(gyr_queue_, gyr_buf);
        }
        pthread_mutex_unlock(&gyr_mtx_);

        std::queue<imu_msg> imu_buf;
        pthread_mutex_lock(&imu_mtx_);
        if (!imu_queue_.empty()) {
            std::swap(imu_queue_, imu_buf);
        }
        pthread_mutex_unlock(&imu_mtx_);

        std::ofstream accf(dump_path_ + "acc" + imu_file_format_, std::ios::app);
        while (!acc_buf.empty()) {
            acc_msg msg = acc_buf.front();
            acc_buf.pop();
            accf << msg.ts << "," << msg.ax << "," <<msg.ay << "," <<msg.az << std::endl;
        }
        accf.close();

        std::ofstream gyrf(dump_path_ + "gyr" + imu_file_format_, std::ios::app);
        while (!gyr_buf.empty()) {
            gyr_msg msg = gyr_buf.front();
            gyr_buf.pop();
            gyrf << msg.ts << "," << msg.rx << "," <<msg.ry << "," <<msg.rz << std::endl;
        }
        gyrf.close();

        std::ofstream imuf(dump_path_ + "imu" + imu_file_format_, std::ios::app);
        while (!imu_buf.empty()) {
            imu_msg msg = imu_buf.front();
            imu_buf.pop();
            imuf << msg.ts << "," << msg.acc_part.ax << "," << msg.acc_part.ay << ","
                 << msg.acc_part.az << ","
                 << msg.gyro_part.rx << "," << msg.gyro_part.ry << "," << msg.gyro_part.rz
                 << std::endl;
        }
        imuf.close();
    }
}
