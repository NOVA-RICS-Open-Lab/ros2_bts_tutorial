#include "rclcpp/rclcpp.hpp"
#include "bt_tutorial_02.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <cstdlib>
#include <string>

#define GROOT_CMD "groot2"

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial") + "/config/bt_tutorial_02_tree.xml";
  BehaviorTreeFactory factory;
  factory.registerNodeType<SaySomething>("SaySomething");
  factory.registerNodeType<ThinkWhatToSay>("ThinkWhatToSay");
  
  // Launch Groot in background
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  auto tree = factory.createTreeFromFile(bt_xml);

  NodeStatus status = NodeStatus::RUNNING;
  while(rclcpp::ok() && status == NodeStatus::RUNNING){
    status = tree.tickRoot();
  }

  return 0;
}
