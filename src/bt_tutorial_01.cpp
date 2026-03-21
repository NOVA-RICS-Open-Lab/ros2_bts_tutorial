#include "bt_tutorial_01.hpp"
#include <cstdlib>
#include "ament_index_cpp/get_package_share_directory.hpp"
// TODO: #include "rclcpp/rclcpp.hpp"

#define GROOT_CMD "groot2"

/*
Tutorial 1 Behavior Tree Explanation:
- We have defined 3 Action Nodes and 1 Condition Node
- The BT follows a Sequence Logic, meaning we only proceed between nodes when the node we are in returns SUCCESS
- The given code was prepared so that all nodes return SUCCESS, which means all of them will execute.
- If any of the nodes fails the BT stops.

File structure:
- bt_tutorial_01.cpp registers all nodes and launches the BT, it imports the bt_tutorial_01.hpp file and uses the bt_tutorial_01_tree.xml
- bt_tutorial_01.hpp codes the actual nodes, their constructors and classes, returning NodeStatus based on their intended behavior
- bt_tutorial_01_tree.xml architects the BT aligned with the programmed nodes, it declares TreeNodesModel (for Groot2 visualization) and BehaviorTree (for BT execution)

TODOs List:
- bt_tutorial_01.cpp replace tree.tickRoot()
- bt_tutorial_01.hpp play with NodeStatus returns
*/

int main() {
  // Define relative path to respective Behavior Tree .xml file
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial_solutions") + "/config/bt_tutorial_01_tree.xml";

  // We use the BehaviorTreeFactory to register our custom nodes
  // The recommended way to create a Node is through inheritance
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<ApproachObject>("ApproachObject");
  
  // Register a SimpleActionNode using a function pointer
  factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));

  // You can also create SimpleActionNodes using methods of a class
  // First we instanciate the class object
  GripperInterface gripper;

  // Then we register the actions in our BehaviorTreeFactory
  factory.registerSimpleAction(
    "OpenGripper",
    std::bind(&GripperInterface::open, &gripper)
  );

  factory.registerSimpleAction(
    "CloseGripper",
    std::bind(&GripperInterface::close, &gripper)
  );

  // Optionally we can launch Groot2 automatically to visualize our BT
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  // Finally we create our tree from the .xml file
  // Trees are created at deployment-time only
  auto tree = factory.createTreeFromFile(bt_xml);

  // To "run" a Tree you need to "tick" it
  // The tick is propagated from the paretn nodes to the children based on the logic of the tree
  tree.tickRoot();

  // TODO: Try replacing tree.tickRoot() with a loop that ticks the BT while inspecting status transition
  /*
  BT::NodeStatus status = BT::NodeStatus::RUNNING;
  while(rclcpp::ok() && status == TODO) {
    status = TODO
  }
  */

  return 0;
}
