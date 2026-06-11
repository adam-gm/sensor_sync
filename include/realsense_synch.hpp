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
        RealsenseSynch(ros::NodeHandle &n, ros::NodeHandle& pnh) : imageSync(left_sub, right_sub, 30)
        {
            std::string left_topic;
            std::string right_topic;
            std::string ic_topic;
            std::string left_output_topic;
            std::string right_output_topic;

            pnh.param<std::string>("left_topic", left_topic,
                           "/camera/infra1/image_rect_raw");

            pnh.param<std::string>("right_topic", right_topic,
                           "/camera/infra2/image_rect_raw");

            pnh.param<std::string>("ic_topic", ic_topic,
                           "/senti/senti/ic");

            pnh.param<std::string>("left_output_topic", left_output_topic,
                           "synched/infra1/img_rect_raw");

            pnh.param<std::string>("right_output_topic", right_output_topic,
                           "synched/infra2/img_rect_raw");

            left_sub.subscribe(n, left_topic, 30);
            right_sub.subscribe(n, right_topic, 30);
            imageSync.registerCallback(boost::bind(&RealsenseSynch::stereoImageCallback, this, _1, _2));
            sub_senti = n.subscribe(ic_topic, 100, &RealsenseSynch::sentiSynchImageCallback, this);

            // Publishers
            left_synch_pub = n.advertise<sensor_msgs::Image>(left_output_topic,30);
            right_synch_pub = n.advertise<sensor_msgs::Image>(right_output_topic,30);
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