#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Header.h>
#include <sensor_msgs/Image.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <deque>


namespace lucid_synch
{
    typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image,
    sensor_msgs::Image
    > StereoSynch;

    class LucidSynch
    {

    public:
        LucidSynch(ros::NodeHandle &n, ros::NodeHandle& pnh) : imageSynch(StereoSynch(30),left_sub, right_sub)
        {
            std::string left_topic;
            std::string right_topic;
            std::string ic1_topic;
            std::string ic2_topic;
            std::string left_output_topic;
            std::string right_output_topic;

            pnh.param<std::string>("left_topic", left_topic,
                           "/lucid1/arena_camera_node/image_raw");

            pnh.param<std::string>("right_topic", right_topic,
                           "/lucid2/arena_camera_node/image_raw");

            pnh.param<std::string>("ic1_topic", ic1_topic,
                           "/senti/senti/lucid1/ic");

            pnh.param<std::string>("ic2_topic", ic2_topic,
                           "/senti/senti/lucid2/ic");

            pnh.param<std::string>("left_output_topic", left_output_topic,
                           "synched/lucid1/img");

            pnh.param<std::string>("right_output_topic", right_output_topic,
                           "synched/lucid2/img");

            left_sub.subscribe(n, left_topic, 30);
            right_sub.subscribe(n, right_topic, 30);
            imageSynch.registerCallback(boost::bind(&LucidSynch::stereoImageCallback, this, _1, _2));
            sub_senti_ic1 = n.subscribe(ic1_topic, 100, &LucidSynch::lucid1_ic_Callback, this);
            sub_senti_ic2 = n.subscribe(ic2_topic, 100, &LucidSynch::lucid2_ic_Callback, this);

            // Publishers
            left_synch_pub = n.advertise<sensor_msgs::Image>(left_output_topic,30);
            right_synch_pub = n.advertise<sensor_msgs::Image>(right_output_topic,30);
        }   


    private:
        // Subscribers
        message_filters::Subscriber<sensor_msgs::Image> left_sub;
        message_filters::Subscriber<sensor_msgs::Image> right_sub;
        //message_filters::TimeSynchronizer<sensor_msgs::Image, sensor_msgs::Image> imageSynch;
        message_filters::Synchronizer<StereoSynch> imageSynch;
        ros::Subscriber sub_senti_ic1;
        ros::Subscriber sub_senti_ic2;
        ros::Publisher left_synch_pub;
        ros::Publisher right_synch_pub;
        
        //Senti_stamp_variable
        //ros::Time senti_lucid1_stamp;
        //ros::Time senti_lucid2_stamp;
        struct SentiHeader {
            ros::Time stamp;
            uint32_t seq;
        };

        std::deque<SentiHeader> lucid1_q;
        std::deque<SentiHeader> lucid2_q;

        void stereoImageCallback(const sensor_msgs::ImageConstPtr &left,
                                 const sensor_msgs::ImageConstPtr &right)
        {

            if (!lucid1_q.empty() && !lucid2_q.empty())
            {
                sensor_msgs::Image left_img = *left;
                sensor_msgs::Image right_img = *right;

                left_img.header.stamp = lucid1_q.front().stamp + ros::Duration(0.0025);
                left_img.header.seq = lucid1_q.front().seq;

                right_img.header.stamp = lucid2_q.front().stamp + ros::Duration(0.0025);
                right_img.header.seq = lucid2_q.front().seq;

                lucid1_q.pop_front();
                lucid2_q.pop_front();

                left_synch_pub.publish(left_img);
                right_synch_pub.publish(right_img);
            }

            else
            {
                ROS_WARN_THROTTLE(1.0, "Missing either 1 or both sentiboard ic timestamps for lucid");
                return;
            }
            
        }

        void lucid1_ic_Callback(const std_msgs::Header::ConstPtr &msg)
        {
            lucid1_q.push_back({msg->stamp, msg->seq});
        }

        void lucid2_ic_Callback(const std_msgs::Header::ConstPtr &msg)
        {
            lucid2_q.push_back({msg->stamp, msg->seq});
        }
    };
}