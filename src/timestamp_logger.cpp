#include <std_msgs/String.h>
#include "timestamp_logger.hpp"


int main(int argc, char **argv){
    ros::init(argc, argv,"realsense_synch_node");
    ros::NodeHandle n;


    TimestampLogger timestamp_logger(n);
    


    ros::spin();

    return 0;
}