cmake_minimum_required(VERSION 2.8.3)
project(lla2enu)

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp std_msgs geodesy sensor_msgs tf nav_msgs message_generation message_filters)

add_service_files(
  FILES
  ComputeDistance.srv
)

generate_messages(
  DEPENDENCIES
  std_msgs
)




## Declare a catkin package
catkin_package(CATKIN_DEPENDS message_runtime message_filters)

## Build talker and listener
include_directories(include ${catkin_INCLUDE_DIRS})



add_executable(sub_car src/sub_car.cpp)
target_link_libraries(sub_car ${catkin_LIBRARIES})



add_executable(sub_obs src/sub_obs.cpp)
target_link_libraries(sub_obs ${catkin_LIBRARIES})



add_executable(compute_distance src/compute_distance.cpp)
target_link_libraries(compute_distance ${catkin_LIBRARIES})
add_dependencies(compute_distance ${catkin_EXPORTED_TARGETS})

add_executable(client_dist src/client.cpp)
target_link_libraries(client_dist ${catkin_LIBRARIES})
add_dependencies(client_dist ${catkin_EXPORTED_TARGETS})