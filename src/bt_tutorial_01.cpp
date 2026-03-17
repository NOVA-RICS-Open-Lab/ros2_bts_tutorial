#include "bt_tutorial_01.hpp"
#include <cstdlib>
#include "ament_index_cpp/get_package_share_directory.hpp"

#define GROOT_CMD "groot2"

int main()
{
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial") + "/config/bt_tutorial_01_tree.xml";
  
  // We use the BehaviorTreeFactory to register our custom nodes
  BehaviorTreeFactory factory;

  // The recommended way to create a Node is through inheritance.
  factory.registerNodeType<ApproachObject>("ApproachObject");

  // Registering a SimpleActionNode using a function pointer.
  // Here we prefer to use a lambda,but you can use std::bind too
  factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));

  // You can also create SimpleActionNodes using methods of a class.
  GripperInterface gripper;

  factory.registerSimpleAction(
      "OpenGripper",
      std::bind(&GripperInterface::open, &gripper));

  factory.registerSimpleAction(
      "CloseGripper",
      std::bind(&GripperInterface::close, &gripper));

  // Launch Groot in background
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  // Trees are created at deployment-time (i.e. at run-time, but only 
  // once at the beginning). 
    
  // IMPORTANT: when the object "tree" goes out of scope, all the 
  // TreeNodes are destroyed
  auto tree = factory.createTreeFromFile(bt_xml);

  // To "execute" a Tree you need to "tick" it.
  // The tick is propagated to the children based on the logic of the tree.
  // In this case, the entire sequence is executed, because all the children
  // of the Sequence return SUCCESS.
  tree.tickRoot();

  return 0;
}
