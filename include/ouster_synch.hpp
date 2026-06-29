#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <deque>
#include <std_msgs/Header.h>
#include <cstdint>
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
        ros::Publisher updatedPointCloud;

        //std::deque<ros::Time> timestampList;
        
        std::deque<sensor_msgs::PointCloud2> pointcloudList;

        struct SentiTime{
            ros::Time stamp;
            std::uint32_t seq;
        };

        std::deque<SentiTime> timestampList;

        ros::Time rosTime;
        ros::Time rosTimeOuster;
        sensor_msgs::PointCloud2 copiedPointCloud;

        
        void lidarSynchCallback(const sensor_msgs::PointCloud2ConstPtr &pointCloud)
        {
            rosTimeOuster = ros::Time::now();
            //If we have a sentiboard time stamp, safely give it to received pointcloud
            if(!timestampList.empty())
            {
                copiedPointCloud = *pointCloud;
                copiedPointCloud.header.stamp = timestampList.front().stamp;
                copiedPointCloud.header.seq = timestampList.front().seq;
                timestampList.pop_front();
                updatedPointCloud.publish(copiedPointCloud);
            }
            //If no sentiboard timestamp exists, give warning and save last pointcloud.
            else
            {
                ROS_WARN_STREAM("Warning, no sentiboard timestamp for current ouster lidar pointcloud exists.");
                pointcloudList.clear();
                pointcloudList.push_back(*pointCloud);
                return;
            }
            
            
        }
        void sentiSynchCallback(const std_msgs::Header::ConstPtr &msg)
        {
            rosTime = ros::Time::now();
            

            if(!pointcloudList.empty())
            {
                if((rosTime - rosTimeOuster).toSec() < 0.05)
                {
                    pointcloudList.front().header.stamp = msg->stamp;
                    //ROS_INFO_STREAM("Successfully gave a late sentiboard timestamp to a earlier received lidar pointcloud");
                    updatedPointCloud.publish(pointcloudList.front());
                    pointcloudList.pop_front();

                    ROS_INFO_STREAM("Late timestamp dt = " << (rosTime - rosTimeOuster).toSec());

                    
                }
                
                else
                {
                    ROS_WARN_STREAM("Warning: Earlier received lidar pointcloud and newly received sentiboard timestamp has too large difference in time. Throwing away pointcloud.");
                    pointcloudList.pop_front();
                }

            }

            else
            {
                timestampList.push_back({msg->stamp, msg->seq});
                while(timestampList.size() > 20)
                {
                    timestampList.pop_front();
                }
            }
        }

    };



}
