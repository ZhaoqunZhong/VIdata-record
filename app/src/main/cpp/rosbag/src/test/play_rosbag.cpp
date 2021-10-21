/*****************************************************************************
 *  Orbbec ob_vislam
 *  Copyright (C) 2020 by ORBBEC Technology., Inc.
 *
 *  This file is part of Orbbec ob_vislam.
 *
 *  This file belongs to ORBBEC Technology., Inc.
 *  It is considered a trade secret, and is not to be divulged or used by
 * parties who have NOT received written authorization from the owner.
 *
 *  Description
 ****************************************************************************/

#include "RosbagStorage/play_rosbag.h"
#include "CvBridgeSimple/cv_bridge_simple.h"
#include <glog/logging.h>
#include <time.h>

using namespace std;
using namespace ob_slam;

PlayRosbag::PlayRosbag(std::shared_ptr<message_flow::MessageFlow>& flow, SystemConfigConstPtr config)
    : flow_(flow)
    , config_(config)
{
    stop_play_ = false;
    bag_.open(config_->ros_bag_file, static_cast<uint32_t>(rosbag::BagMode::Read));
    if (config_->bag_from_to.empty()) {
        t_start_ = ob_slam::TIME_MIN;
        t_end_ = ob_slam::TIME_MAX;
    } else {
        rosbag::View::iterator iter = rosbag::View(bag_, rosbag::TopicQuery(config_->camera_topics[kCamId]),
            ob_slam::TIME_MIN, ob_slam::TIME_MAX)
                                          .begin();
        t_start_ = iter->getTime();
        t_end_ = iter->getTime();
        cout << "camera base start time: " << t_end_ << endl;
        ob_slam::Duration time_from(config_->bag_from_to[0]);
        t_start_ += time_from;
        cout << "camera start time: " << t_start_ << endl;
        ob_slam::Duration time_to(config_->bag_from_to[1]);
        t_end_ += time_to;
        cout << "camera end time: " << t_end_ << endl;
    }

    pub_image_0 = flow_->registerPublisher<message_flow_topics::camera0>();
    pub_image_1 = flow_->registerPublisher<message_flow_topics::camera1>();
    //pub_imu_ = flow_->registerPublisher<message_flow_topics::imu>();
}

void PlayRosbag::PlayTopics()
{
    std::vector<std::string> topics;
    topics.push_back(config_->camera_topics[0]);
    if (2 == config_->camera_num) {
        topics.push_back(config_->camera_topics[1]);
    }
    topics.push_back(config_->imu_topic);
    rosbag::View view(bag_, rosbag::TopicQuery(topics), t_start_, t_end_);
    rosbag::View::iterator iter, next_iter;
    for (iter = view.begin(), next_iter = view.begin(); iter != view.end(); iter++) {
        if (stop_play_)
            break;

        if (config_->camera_topics[0] == iter->getTopic() && iter->isType<sensor_msgs::Image>()) {
            // cout << "左目----"  << endl;
            sensor_msgs::ImageConstPtr image_ptr = iter->instantiate<sensor_msgs::Image>();
            pub_image_0(image_ptr);
        }

        if (2 == config_->camera_num && config_->camera_topics[1] == iter->getTopic() && iter->isType<sensor_msgs::Image>()) {
            // cout << "右目++++"  << endl;
            sensor_msgs::ImageConstPtr image_ptr = iter->instantiate<sensor_msgs::Image>();
            pub_image_1(image_ptr);
        }
        // if (image_ptr != NULL)
        // {
        //     //CvBridgeSimple cbs;
        //     //cv::Mat cv_img = cbs.ConvertToCvMat(image_ptr);
        //     //cv::imshow("img", cv_img);
        //     //cv::waitKey(1);
        //     pub_image_0(image_ptr);
        // }

        sensor_msgs::Imu::ConstPtr imu_ptr = iter->instantiate<sensor_msgs::Imu>();
        if (imu_ptr != NULL) {
            // fprintf(stdout, "imu timestamp: %lf \n", imu_ptr->header.stamp.toSec());
            pub_imu_(imu_ptr);
        }

        if (++next_iter != view.end()) {
            //预留帧间时间间隔
            double sleep_time = next_iter->getTime().toSec() - iter->getTime().toSec();
            sleep_time = sleep_time / config_->play_rate;
            struct timespec ts;
            ts.tv_sec = (long)sleep_time;
            ts.tv_nsec = (long)((sleep_time - (long)sleep_time) * 1e9);
            nanosleep(&ts, 0);
        }
    }
    cout << "bag 播放结束。" << endl;
    stop_play_ = true;
}

void PlayRosbag::Start()
{
    stop_play_ = false;
    cout << "开始播放bag。" << endl;
    play_topics_ = std::thread(&PlayRosbag::PlayTopics, this);
}

void PlayRosbag::Stop()
{
    stop_play_ = true;
}

PlayRosbag::~PlayRosbag()
{
    stop_play_ = true;
    if (play_topics_.joinable()) {
        play_topics_.join();
    }
}

bool PlayRosbag::IsStop()
{
    return stop_play_;
}
