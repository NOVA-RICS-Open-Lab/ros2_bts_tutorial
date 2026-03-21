#include "rclcpp/rclcpp.hpp"
#include "bt_tutorial_02.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <cstdlib>
#include <string>

#define GROOT_CMD "groot2"

/*
Tutorial 2 Behavior Tree Explanation:
- We have defined 2 Action Nodes
- The BT follows a Sequence Logic, meaning we only proceed between nodes when the node we are in returns SUCCESS
- The given code was prepared so that all nodes return SUCCESS, which means all of them will execute.
- If any of the nodes fails the BT stops.
- ThinkWhatToSay node outputs the variable the_answer which the SaySomething node receives as input.

File structure:
- bt_tutorial_02.cpp registers all nodes and launches the BT, uses nodes from the bt_shared_nodes.cpp file, it imports the bt_tutorial_02.hpp file and uses the bt_tutorial_02_tree.xml
- bt_tutorial_02.hpp codes input and output port nodes, their constructors and classes, sharing data across both nodes through a shared Publisher
- bt_tutorial_02_tree.xml architects the BT aligned with the programmed nodes, it declares TreeNodesModel (for Groot2 visualization) and BehaviorTree (for BT execution)

TODOs List:
- bt_tutorial_02.xml intercept message data overwriting it using SetBlackboard
*/

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);

  // Define relative path to respective Behavior Tree .xml file
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial_solutions") + "/config/bt_tutorial_02_tree.xml";
    
  // We use the BehaviorTreeFactory to register our custom nodes
  // The recommended way to create a Node is through inheritance
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<SaySomething>("SaySomething");
  factory.registerNodeType<ThinkWhatToSay>("ThinkWhatToSay");

  // Optionally we can launch Groot2 automatically to visualize our BT
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  // Finally we create our tree from the .xml file
  // Trees are created at deployment-time only
  auto tree = factory.createTreeFromFile(bt_xml);

  // And we now tick the tree in a loop monitoring NodeStatus
  BT::NodeStatus status = BT::NodeStatus::RUNNING;
  while(rclcpp::ok() && status == BT::NodeStatus::RUNNING) {
    status = tree.tickRoot();
  }

  rclcpp::shutdown();
  return 0;
}
