#include "rclcpp/rclcpp.hpp"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "bt_tutorial_05.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <cstdlib>
#include <string>

#define GROOT_CMD "groot2"

/*
Tutorial 5 Behavior Tree Explanation:
- This tutorial introduces SubTrees, meaning we can split a bigger Behavior Tree into smaller reusable trees
- The main tree checks if the door is already open and, if not, calls a subtree responsible for opening it
- The provided starter version uses the simpler DoorClosed subtree, while the advanced exercise extends it with password decoding and result logging
- Fallback, Inverter and RetryUntilSuccessful are used together to create alternative ways of opening the door

File structure:
- bt_tutorial_05.cpp registers all nodes and launches the BT, uses nodes from the bt_shared_nodes.cpp file, it imports the bt_tutorial_05.hpp file and uses the bt_tutorial_05_tree.xml
- bt_tutorial_05.hpp declares the helper class and custom nodes used by the main tree and subtrees
- bt_tutorial_05_tree.xml architects the main BT aligned with the programmed nodes, it includes the subtree XML files and declares TreeNodesModel (for Groot2 visualization)
- bt_tutorial_05_subtree_doorclosed.xml and the other subtree XML files define the reusable subtrees executed by the main tree

TODOs List:
- bt_tutorial_05.cpp register the extra password/result nodes using builder functions and shared state when needed
- bt_tutorial_05_doorclosed.xml edit the door closed subtree to call the password-based subtree instead of the simpler open/pick/smash sequence
- bt_tutorial_05_subtree_decode.xml create this subtree file
*/

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);

  // Define relative path to respective Behavior Tree .xml file
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial_solutions") + "/config/bt_tutorial_05_tree.xml";
  
  // We use the BehaviorTreeFactory to register our custom nodes
  // The recommended way to create a Node is through inheritance
  BT::BehaviorTreeFactory factory;

  auto context = std::make_shared<DoorExecutionContext>();
  
  CrossDoor cross_door(context);
  cross_door.registerNodes(factory);
  
  registerPasswordNodes(factory, cross_door, context);

  // Optionally we can launch Groot2 automatically to visualize our BT
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  // Finally we create our tree from the .xml file
  // Trees are created at deployment-time only
  auto tree = factory.createTreeFromFile(bt_xml);

  // Helper function to print the tree
  BT::printTreeRecursively(tree.rootNode());

  // And we now tick the tree in a loop monitoring NodeStatus
  BT::NodeStatus status = BT::NodeStatus::RUNNING;
  while(rclcpp::ok() && status == BT::NodeStatus::RUNNING) {
    status = tree.tickRoot();
  }

  rclcpp::shutdown();
  return 0;
}
