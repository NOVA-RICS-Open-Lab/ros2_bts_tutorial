#include "rclcpp/rclcpp.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "bt_tutorial_05.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <cstdlib>
#include <string>

#define GROOT_CMD "groot2"

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial") + "/config/bt_tutorial_05_tree.xml";
  
  BT::BehaviorTreeFactory factory;

  CrossDoor cross_door;
  cross_door.registerNodes(factory);

  // Launch Groot in background
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  // the XML is the one shown at the beginning of the tutorial
  auto tree = factory.createTreeFromFile(bt_xml);

  // helper function to print the tree
  BT::printTreeRecursively(tree.rootNode());

  // tree.tickRootWhileRunning();

  BT::NodeStatus status = BT::NodeStatus::RUNNING;
  while(rclcpp::ok() && status == BT::NodeStatus::RUNNING){
    status = tree.tickRoot();
  }

  return 0;
}
