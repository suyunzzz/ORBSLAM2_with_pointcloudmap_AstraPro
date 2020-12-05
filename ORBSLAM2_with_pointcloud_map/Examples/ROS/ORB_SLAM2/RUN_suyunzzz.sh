echo "---RUN---"

gnome-terminal -t " roscore" -x bash -c " sh ./roscore.sh;exec bash;"

gnome-terminal -t " orbslam2rgbd" -x bash -c " sh ./run_bag_rgbd.sh;exec bash;"

gnome-terminal -t " rosbag_play" -x bash -c " sh ./rosbag_play.sh;exec bash;"
