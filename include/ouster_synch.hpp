#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>

namespace ouster_synch
{

    class OusterSynch
    {
    public:
        OusterSynch(ros::NodeHandle &n)
        {
        lidar_points = n.subscribe("/ouster/points", 100, &OusterSynch::lidarSynchCallback, this);
        senti_lidar_ic = n.subscribe("/senti/senti/lidar/ic", 100, &OusterSynch::sentiSynchCallback, this);
        updatedPointCloud = n.advertise<sensor_msgs::PointCloud2>("synched/ouster/points", 100);
        }
    private:
        ros::Subscriber lidar_points;
        ros::Subscriber senti_lidar_ic;
        ros::Time senti_stamp;
        ros::Publisher updatedPointCloud;


        void lidarSynchCallback(const sensor_msgs::PointCloud2ConstPtr &pointCloud)
        {
            sensor_msgs::PointCloud2 copiedPointCloud = *pointCloud;
            copiedPointCloud.header.stamp = senti_stamp;
            updatedPointCloud.publish(copiedPointCloud);
        }
        void sentiSynchCallback(const std_msgs::Header::ConstPtr &msg)
        {
            senti_stamp = msg->stamp;
        }

    };



}