#include <ros/ros.h>
#include <sensor_msgs/Image.h>

#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

#include <fstream>

class TimestampLogger
{
public:
    typedef message_filters::sync_policies::ApproximateTime<
        sensor_msgs::Image,
        sensor_msgs::Image,
        sensor_msgs::Image
    > SyncPolicy;

    TimestampLogger(ros::NodeHandle& nh)
        : infra1_sub(nh, "/camera/infra1/image_rect_raw", 10),
          infra2_sub(nh, "/camera/infra2/image_rect_raw", 10),
          color_sub(nh, "/camera/color/image_raw", 10),
          sync(SyncPolicy(20), infra1_sub, infra2_sub, color_sub)
    {
        file.open("/workspace/timestamps.csv");

        file << "infra1_sec,infra1_nsec,"
             << "infra2_sec,infra2_nsec,"
             << "color_sec,color_nsec\n";
        file.flush();

        sync.registerCallback(
            boost::bind(
                &TimestampLogger::callback,
                this,
                _1,
                _2,
                _3));
    }

private:
    void callback(
        const sensor_msgs::ImageConstPtr& infra1,
        const sensor_msgs::ImageConstPtr& infra2,
        const sensor_msgs::ImageConstPtr& color)
    {
        ROS_INFO("Timestamp callback triggered");

        file
            << infra1->header.stamp.sec << ","
            << infra1->header.stamp.nsec << ","

            << infra2->header.stamp.sec << ","
            << infra2->header.stamp.nsec << ","

            << color->header.stamp.sec << ","
            << color->header.stamp.nsec
            << "\n";

        file.flush();
    }

    std::ofstream file;

    message_filters::Subscriber<sensor_msgs::Image> infra1_sub;
    message_filters::Subscriber<sensor_msgs::Image> infra2_sub;
    message_filters::Subscriber<sensor_msgs::Image> color_sub;

    message_filters::Synchronizer<SyncPolicy> sync;
};