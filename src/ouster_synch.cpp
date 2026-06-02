#include "ouster_synch.hpp"

using namespace ouster_synch;

int main(int argc, char **argv){
    ros::init(argc, argv,"ouster_synch_node");
    ros::NodeHandle n;
    OusterSynch senti_ouster_node(n);
    


    ros::spin();

    return 0;
}
