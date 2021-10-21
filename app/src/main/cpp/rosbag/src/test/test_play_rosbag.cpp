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

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <iostream>

#include "RosbagStorage/play_rosbag.h"

using namespace std;
//using namespace ob_slam;
//using namespace message_flow;

DECLARE_string(yaml_config_dir);

TEST(test_system_common, test_play_rosbag)
{
    //string config_file = "/home/zengjie/work/vio/OB_VISLAM/SystemCommon/test/test_config.yaml";
    string config_file = FLAGS_yaml_config_dir + "/test_config.yaml";
    cout << "config_file: " << config_file << endl;
    SystemConfigConstPtr slam_config(new SystemConfig(config_file));

    std::shared_ptr<MessageFlow> flow(
        MessageFlow::create<MessageDispatcherFifo>(
            std::thread::hardware_concurrency()));

    PlayRosbag* player(new PlayRosbag(flow, slam_config));
    player->Start();
    sleep(2);
    player->Stop();
    delete player;
}
