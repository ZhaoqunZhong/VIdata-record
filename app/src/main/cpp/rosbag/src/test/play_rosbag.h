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

#ifndef __OB_VISLAM_PLAY_ROSBAG_H__
#define __OB_VISLAM_PLAY_ROSBAG_H__

#include "MessageType/rostime/time.h"
#include "MessageType/sensor_msgs/Image.h"
#include "MessageType/sensor_msgs/Imu.h"
#include "RosbagStorage/rosbag/bag.h"
#include "RosbagStorage/rosbag/view.h"
//#include "SystemCommon/global.h"
//#include "SystemCommon/system_config.h"
#include "message_flow/message-flow.h"
#include <memory>

namespace ob_slam {
class PlayRosbag {
public:
    PlayRosbag(std::shared_ptr<message_flow::MessageFlow>& flow, SystemConfigConstPtr config);

    ~PlayRosbag();

    void Start();

    void Stop();

    bool IsStop();

private:
    void PlayTopics();

    std::shared_ptr<message_flow::MessageFlow> flow_;
    SystemConfigConstPtr config_;

    bool stop_play_;
    rosbag::Bag bag_;
    ob_slam::Time t_start_;
    ob_slam::Time t_end_;

    std::function<void(const sensor_msgs::ImageConstPtr&)> pub_image_0;
    std::function<void(const sensor_msgs::ImageConstPtr&)> pub_image_1;
    std::function<void(const sensor_msgs::ImuConstPtr&)> pub_imu_;

    std::thread play_topics_;
};
}

#endif //__OB_VISLAM_PLAY_ROSBAG_H__
