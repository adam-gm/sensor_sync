cmake_minimum_required(VERSION 3.0.2)
project(senti_sensor_synch)

## Find catkin and any other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  sensor_msgs
  message_filters
  # Add other dependencies here
)

# Find librealsense2 installed package
find_package(realsense2 REQUIRED)

## Declare ROS messages, services, and actions (if any)
# add_message_files(FILES Message1.msg)
# add_service_files(FILES Service1.srv)
# generate_messages(DEPENDENCIES std_msgs)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

## Declare catkin package configuration
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES my_package
  CATKIN_DEPENDS roscpp std_msgs sensor_msgs message_filters
)

## Build Directory
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  #${CMAKE_CURRENT_SOURCE_DIR}/include
)

## Declare C++ executable
add_executable(realsense_synch_node src/realsense_synch.cpp)
add_executable(realsense_custom_config src/realsense_custom_config.cpp)
## Specify link libraries for the executable
target_link_libraries(realsense_synch_node
  ${catkin_LIBRARIES}
)

target_link_libraries(realsense_custom_config
  ${realsense2_LIBRARY}
)

## Installation (for make install)
install(TARGETS realsense_synch_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)