#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <deque>

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
        std::deque<ros::Time> timestampList;
        std::deque<sensor_msgs::PointCloud2> pointcloudList;
        ros::Time rosTime;
        sensor_msgs::PointCloud2 copiedPointCloud;

        void lidarSynchCallback(const sensor_msgs::PointCloud2ConstPtr &pointCloud)
        {
            //If we have a sentiboard time stamp, safely give it to received pointcloud
            if(timestampList.size()>0)
            {
                copiedPointCloud = *pointCloud;
                copiedPointCloud.header.stamp = timestampList.front();
                timestampList.pop_front();
                updatedPointCloud.publish(copiedPointCloud);
            }
            //If not, give warning and save last pointcloud.
            else
            {
                ROS_WARN_STREAM("Warning, no sentiboard timestamp for current ouster lidar pointcloud exists.");
                if(pointcloudList.size()==0)
                {
                    pointcloudList.push_back(*pointCloud);
                    return;
                }
                else
                {
                    pointcloudList.pop_front();
                    pointcloudList.push_back(*pointCloud);
                    return;
                }
            
            }
        }
        void sentiSynchCallback(const std_msgs::Header::ConstPtr &msg)
        {
            //senti_stamp = msg->stamp;
            rosTime = ros::Time::now().toSec();
            timestampList.push_back(msg->stamp);

            if(pointcloudList.size()>0)
            {
                copiedPointCloud = pointcloudList.front();
                copiedPointCloud = pointcloudList.pop_front();
                if((rosTime - copiedPointCloud.header.stamp.toSec()) < 0.05)
                {
                    copiedPointCloud.header.stamp = timestampList.front();
                    timestampList.pop_front();
                    ROS_INFO_STREAM("Successfully gave a late sentiboard timestamp to a earlier received lidar pointcloud");
                    updatedPointCloud.publish(copiedPointCloud);
                    
                }
                
                else
                {
                    ROS_WARN_STREAM("Warning: Earlier received lidar pointcloud and newly received sentiboard timestamp has too large difference in time. Throwing away pointcloud.")
                }

            }
        }

    };



}