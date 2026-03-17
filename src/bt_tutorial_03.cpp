#include "rclcpp/rclcpp.hpp"
#include "bt_tutorial_03.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

#define GROOT_CMD "groot2"

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  if (argc < 4)
  {
    std::cerr << "Usage: ros2 run ros2_bts_tutorial bt_tutorial_03 <x> <y> <z>" << std::endl;
    return 1;
  }

  const std::string x = argv[1];
  const std::string y = argv[2];
  const std::string z = argv[3];
  const std::string cli_goal = x + ";" + y + ";" + z;
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial") + "/config/bt_tutorial_03_tree.xml";

  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<PrintTarget>("PrintTarget");

  // Launch Groot in background
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  auto blackboard = BT::Blackboard::create();
  blackboard->set("GoalPosition", cli_goal);

  auto tree = factory.createTreeFromFile(bt_xml, blackboard);

  BT::NodeStatus status = BT::NodeStatus::RUNNING;
  while (rclcpp::ok() && status == BT::NodeStatus::RUNNING) {
    status = tree.tickRoot();
  }

  rclcpp::shutdown();
  return 0;
}
