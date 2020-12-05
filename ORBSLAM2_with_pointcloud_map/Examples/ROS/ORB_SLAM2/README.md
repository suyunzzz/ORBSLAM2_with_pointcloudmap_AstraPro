## CHANGES
1.修改ORB_SLAM2源码，更改图片及点云地图显示为彩色，增加自动保存result.pcd

2.**FIX** 运行结束卡死bug

3.**FIX** *Examples/ROS/ORB_SLAM2*编译失败（在*CMakeLists.txt*中加入PCL依赖等即可编译成功）

4.**FIX** PointCloudMapping不完全更新的问题，当发生回环后不能更新校正后的点云地图

5.**ADD** 点云地图PassThrough滤波，去除远处不准确的点

## NOTE
1.编译前需运行：
```bash
	export ROS_PACKAGE_PATH=${ROS_PACKAGE_PATH}:/your/path/ORBSLAM2_with_pointcloud_map/ORB_SLAM2_modified/Examples/ROS
```
否则会出错说找不到路径和包

2.运行
```bash
tools/bin_vocabulary
```
生成二进制字典

3.停止运行病保存生成的轨迹与点云地图，请运行

```bash 
rostopic pub /RGBD/cmd std_msgs/String "data: 'stop'"
```

## How To Build

1. ```export ROS_PACKAGE_PATH=${ROS_PACKAGE_PATH}:/your/path/ORBSLAM2_with_pointcloud_map/ORB_SLAM2_modified/Examples/ROS```

2. ```mkdir build```

3. ```cmake ..```

4. ```make -j4```

## How To Run

1. ```rosbag play -r 1.0 *.bag```
    或使用Kinect
2. ```./RGBD ../../../Vocabulary/ORBvoc.bin kinect2_121717234447.yaml```或`rosrun ORB_SLAM2 RGBD PATH_TO_VOCABULARY PATH_TO_SETTINGS_FILE`

3. ```rostopic pub /RGBD/cmd std_msgs/String "data: 'stop'"``` 停止建图
4. ```pcl_viewer result.pcd```查看pcd文件

---



## 2020-12-4 update

> 将整个流程放在shell命令中

运行方式：进入目录`/home/s/ORBSLAM2_with_pointcloud_map/Examples/ROS/ORB_SLAM2`，执行`./RUN_suyunzzz.sh`

停止建图```rostopic pub /RGBD/cmd std_msgs/String "data: 'stop'"``` 

点云文件`result.pcd`将存放在`/home/s/ORBSLAM2_with_pointcloud_map/Examples/ROS/ORB_SLAM2`下

## 2020-12-5 updata

> 使用AstraPro运行orbslam2
>
> 我们采用的ros的版本，不需要控制底层的数据，简单一些

1、启动AstraPro

```bash
roslaunch astra_camera astrapro.launch 
```

2、进入orbslam2目录，启动orbslam2的rgbd接口，第二个参数为词袋模型，第三个参数为相机的参数+slam的参数

```bash
 ./RGBD ../../../Vocabulary/ORBvoc.bin astra.yaml
```

3、停止运行，并保存

```bash
rostopic pub /RGBD/cmd std_msgs/String "data: 'stop'"
```





