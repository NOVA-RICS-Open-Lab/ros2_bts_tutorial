cmake_minimum_required(VERSION 3.8)
project(ros2_bts_tutorial)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(std_msgs REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)

set(dependencies
  ament_index_cpp
  rclcpp
  rclcpp_action
  std_msgs
  behaviortree_cpp_v3
)  

include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

add_executable(bt_tutorial_01 src/bt_tutorial_01.cpp)
add_executable(bt_tutorial_02 src/bt_tutorial_02.cpp src/bt_shared_nodes.cpp)
add_executable(bt_tutorial_03 src/bt_tutorial_03.cpp)
add_executable(bt_tutorial_04_sequence src/bt_tutorial_04_sequence.cpp)
add_executable(bt_tutorial_04_reactive src/bt_tutorial_04_reactive.cpp)
add_executable(bt_tutorial_05 src/bt_tutorial_05.cpp src/bt_shared_nodes.cpp)

ament_target_dependencies(bt_tutorial_01 ${dependencies})
ament_target_dependencies(bt_tutorial_02 ${dependencies})
ament_target_dependencies(bt_tutorial_03 ${dependencies})
ament_target_dependencies(bt_tutorial_04_sequence ${dependencies})
ament_target_dependencies(bt_tutorial_04_reactive ${dependencies})
ament_target_dependencies(bt_tutorial_05 ${dependencies})


install(TARGETS
  bt_tutorial_01
  bt_tutorial_02
  bt_tutorial_03
  bt_tutorial_04_sequence
  bt_tutorial_04_reactive
  bt_tutorial_05
  DESTINATION lib/${PROJECT_NAME})

install(
  DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
