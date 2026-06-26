#include <ros/ros.h>
#include <std_msgs/String.h>
#include "lucid_synch.hpp"

// Synch cable connected to sentiboard SPI2 port, oc1 (2.7V-3.3V). Sensor 7. Protobuf header().source()=7
using namespace lucid_synch;

int main(int argc, char **argv){
    ros::init(argc, argv,"lucid_synch_node");
    ros::NodeHandle n;

    ros::NodeHandle pnh("~");
    LucidSynch senti_lucid_node(n,pnh);
    


    ros::spin();

    return 0;
}
