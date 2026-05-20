#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <iostream>

int main()
{
    rs2::context ctx;
    rs2::device_list devices = ctx.query_devices();

    if (devices.size() == 0)
    {
        std::cerr << "No RealSense device found \n";
        return 1;
    }

    rs2::device dev = devices.front();

    for (rs2::sensor sensor : dev.query_sensors())
    {
        std::cout << "Checking sensor";
        if (sensor.supports(RS2_CAMERA_INFO_NAME))
        {
            std::cout << sensor.get_info(RS2_CAMERA_INFO_NAME);
        }

        std::cout << std::endl;

        if (sensor.supports(RS2_OPTION_INTER_CAM_SYNC_MODE))
        {
            sensor.set_option(RS2_OPTION_INTER_CAM_SYNC_MODE, 1.f);
            std::cout << "Set INTER_CAM_SYNC_MODE = 1\n";
            float sync_value = sensor.get_option(RS2_OPTION_INTER_CAM_SYNC_MODE);
            std::cout << "Current Sync Mode : " << sync_value << "\n";

        }
        
    
        if (sensor.supports(RS2_OPTION_OUTPUT_TRIGGER_ENABLED))
        {
            sensor.set_option(RS2_OPTION_OUTPUT_TRIGGER_ENABLED, 1.f);
            std::cout << "Set OUTPUT_TRIGGER_ENABLED = 1\n";
            float trigger_value = sensor.get_option(RS2_OPTION_OUTPUT_TRIGGER_ENABLED);
            std::cout << "Current Output Trigger Enabled Value: " << trigger_value << "\n";

        }


    }

    return 0;
}