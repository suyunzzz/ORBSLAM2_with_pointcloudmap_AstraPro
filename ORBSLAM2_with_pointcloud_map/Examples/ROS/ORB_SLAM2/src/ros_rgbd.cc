/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/


#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <cv_bridge/cv_bridge.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

#include<opencv2/core/core.hpp>

#include"System.h"

using namespace std;

class ImageGrabber {
public:
    ImageGrabber(ORB_SLAM2::System *pSLAM) : mpSLAM(pSLAM)
    {}

    void GrabRGBD(const sensor_msgs::ImageConstPtr &msgRGB, const sensor_msgs::ImageConstPtr &msgD);

    ORB_SLAM2::System *mpSLAM;
};


void cmdCallback(const std_msgs::String::ConstPtr &cmd)
{   
    cout<<"\n---------------------------"<<endl;
    ROS_INFO("cmd: [%s]", cmd->data.c_str());
    cout<<"\n---------------------------"<<endl;

    if (cmd->data == "stop")
    
        ros::shutdown();
        
}

// // service的回调函数
// bool save_map_callback(saveMap::Request &request, saveMap::Response &response) {
//     response.succeed = true;
//     ros::shutdown();
//     cout<<"[service] call save_map to shutdown ros."<<endl;
//     return response.succeed;
// }

int main(int argc, char **argv)
{
    ros::init(argc, argv, "RGBD");
    ros::start();

    if (argc != 3)
    {
        cerr << endl << "Usage: rosrun ORB_SLAM2 RGBD path_to_vocabulary path_to_settings" << endl;
        ros::shutdown();
        return 1;
    }

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1], argv[2], ORB_SLAM2::System::RGBD, true);
    cv::FileStorage fsSettings(argv[2], cv::FileStorage::READ);
    if (!fsSettings.isOpened())
    {
        cerr << "Failed to open settings file at: " << argv[2] << endl;
        exit(-1);
    }

    string rgb_topic(fsSettings["rgb_topic"]);
    string depth_topic(fsSettings["depth_topic"]);
    if (rgb_topic.empty())
    {
        rgb_topic = "/camera/rgb/image_color";
    }
    if (depth_topic.empty())
    {
        depth_topic = "/camera/depth/image";
    }

    cout << endl << "------------------------" << endl;
    cout << "- rgb_topic: " << rgb_topic << endl;
    cout << "- depth_topic: " << depth_topic << endl;

    ImageGrabber igb(&SLAM);

    ros::NodeHandle nh;

    message_filters::Subscriber <sensor_msgs::Image> rgb_sub(nh, rgb_topic, 1);
    message_filters::Subscriber <sensor_msgs::Image> depth_sub(nh, depth_topic, 1);

    // 同步
    typedef message_filters::sync_policies::ApproximateTime <sensor_msgs::Image, sensor_msgs::Image> sync_pol;
    message_filters::Synchronizer <sync_pol> sync(sync_pol(10), rgb_sub, depth_sub);
    sync.registerCallback(boost::bind(&ImageGrabber::GrabRGBD, &igb, _1, _2));

    // 订阅停止话题
    ros::Subscriber sub = nh.subscribe("/RGBD/cmd", 1000, cmdCallback);

    // 停止运行 service
    // 创建一个服务器
    // ros::ServiceServer service = nh.advertiseService("save_map", save_map_callback);


    ros::spin();

    // Stop all threads
    SLAM.Shutdown();

    // Save camera trajectory
    SLAM.SaveTrajectoryTUM("CameraTrajectory.txt");
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

    ros::shutdown();

    return 0;
}

void ImageGrabber::GrabRGBD(const sensor_msgs::ImageConstPtr &msgRGB, const sensor_msgs::ImageConstPtr &msgD)
{
    // Copy the ros image message to cv::Mat.
    cv_bridge::CvImageConstPtr cv_ptrRGB;
    try
    {
        cv_ptrRGB = cv_bridge::toCvShare(msgRGB);
    }
    catch (cv_bridge::Exception &e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    cv_bridge::CvImageConstPtr cv_ptrD;
    try
    {
        cv_ptrD = cv_bridge::toCvShare(msgD);
    }
    catch (cv_bridge::Exception &e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    mpSLAM->TrackRGBD(cv_ptrRGB->image, cv_ptrD->image, cv_ptrRGB->header.stamp.toSec());
}
