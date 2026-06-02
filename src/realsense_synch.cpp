#include <ros/ros.h>
#include <std_msgs/String.h>
#include "realsense_synch.hpp"

// Synch cable connected to sentiboard SPI2 port, oc1 (2.7V-3.3V). Sensor 7. Protobuf header().source()=7
using namespace realsense_synch;

int main(int argc, char **argv){
    ros::init(argc, argv,"realsense_synch_node");
    ros::NodeHandle n;
    RealsenseSynch senti_realsense_node(n);
    


    ros::spin();

    return 0;
}
