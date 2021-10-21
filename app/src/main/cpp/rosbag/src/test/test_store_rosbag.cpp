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
#include "MessageType/sensor_msgs/Imu.h"
#include "RosbagStorage/rosbag/bag.h"
#include <gtest/gtest.h>

TEST(test_system_common, test_store_rosbag)
{
    ob_slam::Time::init();
    ob_slam::rosbag::Bag bag;
    bag.open("xxx.bag", static_cast<uint32_t>(ob_slam::rosbag::BagMode::Write));
    ob_slam::sensor_msgs::Imu imu_msg;
    imu_msg.header.frame_id = "imu_base";
    imu_msg.angular_velocity.x = 0.0;
    imu_msg.angular_velocity.y = 0.0;
    imu_msg.angular_velocity.x = 0.0;
    imu_msg.linear_acceleration.x = 0.01;
    imu_msg.linear_acceleration.y = 0.02;
    imu_msg.linear_acceleration.z = 0.03;
    for (int i = 0; i < 1000; ++i) {
        imu_msg.header.stamp = ob_slam::Time::now();
        bag.write("imu", ob_slam::Time::now(), imu_msg);
    }
}
