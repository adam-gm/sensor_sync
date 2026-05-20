#include <ros/ros.h>
#include <std_msgs/String.h>
#include "realsense_synch.hpp"

using namespace realsense_synch;

int main(int argc, char **argv){
    ros::init(argc, argv,"realsense_synch_node");
    ros::NodeHandle n;
    RealsenseSynch senti_realsense_node(n);
    


    ros::spin();

    return 0;
}
