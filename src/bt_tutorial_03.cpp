#include "rclcpp/rclcpp.hpp"
#include "bt_tutorial_03.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

#define GROOT_CMD "groot2"

/*
Tutorial 3 Behavior Tree Explanation:
- We have defined 1 Action Node
- The BT follows a Sequence Logic, meaning we only proceed between nodes when the node we are in returns SUCCESS
- The given code was prepared so that all nodes return SUCCESS, which means all of them will execute.
- If any of the nodes fails the BT stops.
- PrintTarget receives coordinates to print as the goal trough the Blackboard
- SetBlackboard is used to force a new goal hardcoded on the BT .xml file to be printed

File structure:
- bt_tutorial_03.cpp registers all nodes, gets argv values, pre-sets the Blackboard, and launches the BT, it imports the bt_tutorial_03.hpp file and uses the bt_tutorial_03_tree.xml
- bt_tutorial_03.hpp codes the actual nodes, their constructors and classes, sharing data accross both nodes through Blackboard (which is not recommended)
- bt_tutorial_03_tree.xml architects the BT aligned with the programmed nodes, it declares TreeNodesModel (for Groot2 visualization) and BehaviorTree (for BT execution)

TODOs List:
- bt_tutorial_03.cpp create NodeBuilder functions for both CalculateGoal and PrintTarget, properly passing arguments to and from the nodes without using the Blackboard
- bt_tutorial_03.hpp remake PrintTarget and CalculateGoal so they properly receive inputs as arguments instead of getting them from reading the Blackboard
- bt_tutorial_03_tree.xml replace SetBlackboard with CalculateGoal and print its result using PrintTarget
*/

int main(int argc, char **argv) {
  // Initialize argc and argv for arguments capture in command line
  // argc is the number of arguments passed and argv a list of size argc with the actual arguments
  rclcpp::init(argc, argv);
  // We require 4 arguments for the program to execute, the executable name counts as 1 plus the 3 coordinates 
  if (argc != 4) {
    std::cerr << "Usage: ros2 run ros2_bts_tutorial_solutions bt_tutorial_03 <x> <y> <z>" << std::endl;
    return 1;
  }

  // Get argv argument values and assign them to the correct coordinates
  // TODO: Instead of receiving arguments as strings let's change them to doubles (double input_x = std::stod(argv[1]);)
  const std::string x = argv[1];
  const std::string y = argv[2];
  const std::string z = argv[3];

  // Contruct our goal as a string merging x;y;z
  const std::string goal = x + ";" + y + ";" + z;
  // TODO: Instead of a string goal, let's declare goal as a shared Position3D
  // auto goal = std::make_shared<TODO>(TODO);

  // Define relative path to respective Behavior Tree .xml file
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial_solutions") + "/config/bt_tutorial_03_tree.xml";

  // We use the BehaviorTreeFactory to register our custom nodes
  // The recommended way to create a Node is through inheritance
  // TODO: Let's introduce the concept of builder functions, necessary to properly pass arguments to and from Nodes
  // Using he Blackboard is not recommended as it can overwrite any set key and use them accross BTs execution (mainly used for debug)
  BT::BehaviorTreeFactory factory;
  /*
  BT::NodeBuilder builder_PrintTarget =
    [goal](const std::string& name, const BT::NodeConfiguration& config)
    {
      return std::make_unique<PrintTarget>(name, config, goal);
    };
  */
  factory.registerNodeType<PrintTarget>("PrintTarget" /*builder_PrintTarget*/);

  //TODO: Create the builder function for the CalculateGoal node here
  //TODO: Don't forget to register the CalculateGoal

  // Optionally we can launch Groot2 automatically to visualize our BT
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  // Create Blackboard to pre-set our goal as the input cmd arguments
  auto blackboard = BT::Blackboard::create();
  blackboard->set("GoalPosition", goal);

  // Finally we create our tree from the .xml file, but now passing the pre-set Blackboard
  // Trees are created at deployment-time only
  auto tree = factory.createTreeFromFile(bt_xml, blackboard);

  // We tick the tree in a loop monitoring NodeStatus
  BT::NodeStatus status = BT::NodeStatus::RUNNING;
  while(rclcpp::ok() && status == BT::NodeStatus::RUNNING) {
    status = tree.tickRoot();
  }

  rclcpp::shutdown();
  return 0;
}
