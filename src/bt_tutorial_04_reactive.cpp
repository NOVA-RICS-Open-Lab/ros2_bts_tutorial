#include "rclcpp/rclcpp.hpp"
#include "bt_tutorial_04_reactive.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"

#include <cstdlib>
#include <string>
#include <memory>
#include <iostream>

#define GROOT_CMD "groot2"

/*
Tutorial 4 Reactive Sequence Behavior Tree Explanation:
- We have defined 2 Action Nodes and 1 Condition Node
- The BT follows a ReactiveSequence Logic, meaning previous children can be ticked again while a later child is still RUNNING
- This means BatteryOK is checked repeatedly and can interrupt the running branch if it returns FAILURE
- Compare this behavior with the regular Sequence version in bt_tutorial_04_sequence.cpp

File structure:
- bt_tutorial_04_reactive.cpp registers all nodes and launches the BT, it imports the bt_tutorial_04_reactive.hpp file and uses the bt_tutorial_04_reactive_tree.xml
- bt_tutorial_04_reactive.hpp codes the actual nodes, their constructors and classes, returning NodeStatus based on their intended behavior
- bt_tutorial_04_reactive_tree.xml architects the BT aligned with the programmed nodes, it declares TreeNodesModel (for Groot2 visualization) and BehaviorTree (for BT execution)

TODOs List:
- bt_tutorial_04_reactive.hpp make BatteryOK NodeStatus alternate between SUCCESS and FAILURE
*/

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  // Define relative path to respective Behavior Tree .xml file
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial_solutions") + "/config/bt_tutorial_04_reactive_tree.xml";

  // We use the BehaviorTreeFactory to register our custom nodes
  // The recommended way to create a Node is through inheritance
  BT::BehaviorTreeFactory factory;

  factory.registerNodeType<BatteryOK>("BatteryOK");
  factory.registerNodeType<SaySomething>("SaySomething");
  factory.registerNodeType<MoveBaseAction>("MoveBase");

  // Optionally we can launch Groot2 automatically to visualize our BT
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  // Finally we create our tree from the .xml file
  // Trees are created at deployment-time only
  auto tree = factory.createTreeFromFile(bt_xml);

  // And we now tick the tree in a loop monitoring NodeStatus
  BT::NodeStatus status = BT::NodeStatus::IDLE;
  while (rclcpp::ok())
  {
    std::cout << "--- ticking" << std::endl;
    status = tree.tickRoot();
    std::cout << "--- status: " << BT::toStr(status) << "\n" << std::endl;

    if (status == BT::NodeStatus::SUCCESS)
    {
      break;
    }

    tree.sleep(std::chrono::milliseconds(100));
  }

  rclcpp::shutdown();
  return 0;
}
