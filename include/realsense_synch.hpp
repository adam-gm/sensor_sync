#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/Image.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>


namespace realsense_synch
{

    class RealsenseSynch
    {

    public:
        RealsenseSynch(ros::NodeHandle &n) : imageSync(left_sub, right_sub, 30)
        {
            left_sub.subscribe(n, "/camera/infra1/image_rect_raw", 30);
            right_sub.subscribe(n, "/camera/infra2/image_rect_raw", 30);
            imageSync.registerCallback(boost::bind(&RealsenseSynch::stereoImageCallback, this, _1, _2));
            sub_senti = n.subscribe("/senti/senti/ic", 100, &RealsenseSynch::sentiSynchImageCallback, this);

            // Publishers
            left_synch_pub = n.advertise<sensor_msgs::Image>("synched/infra1/img_rect_raw",30);
            right_synch_pub = n.advertise<sensor_msgs::Image>("synched/infra2/img_rect_raw",30);
        }   


    private:
        // Subscribers
        message_filters::Subscriber<sensor_msgs::Image> left_sub;
        message_filters::Subscriber<sensor_msgs::Image> right_sub;
        message_filters::TimeSynchronizer<sensor_msgs::Image, sensor_msgs::Image> imageSync;
        ros::Subscriber sub_senti;
        ros::Publisher left_synch_pub;
        ros::Publisher right_synch_pub;
        
        //Senti_stamp_variable
        ros::Time senti_stamp;

        void stereoImageCallback(const sensor_msgs::ImageConstPtr &left,
                                 const sensor_msgs::ImageConstPtr &right)
        {
            sensor_msgs::Image left_img = *left;
            sensor_msgs::Image right_img = *right;
            left_img.header.stamp = senti_stamp;
            right_img.header.stamp = senti_stamp;

            left_synch_pub.publish(left_img);
            right_synch_pub.publish(right_img);
        }

        void sentiSynchImageCallback(const std_msgs::Header::ConstPtr &msg)
        {
            ROS_INFO("Sentiboard img timestamp in seconds: %f", msg->stamp.toSec());
            senti_stamp = msg->stamp;
        }
    };
}