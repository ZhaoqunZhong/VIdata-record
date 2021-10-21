cmake_minimum_required(VERSION 3.0.2)

project(test_rosbag)

add_executable(${PROJECT_NAME}
#    test_play_rosbag.cpp
    test_store_rosbag.cpp
)

target_link_libraries(${PROJECT_NAME} 
    ${OpenCV_LIBS}
#    glog::glog
    glog
    gflags
    GTest::gtest
    GTest::gtest_main
    RosbagStorage
    CvBridgeSimple
)

#add_test(NAME test_rosbag
#    COMMAND test_rosbag --yaml_config_dir=${CMAKE_CURRENT_LIST_DIR}/../../TestData/TestConfig
#)