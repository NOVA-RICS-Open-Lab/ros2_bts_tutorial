#ifndef ROS2_BT_TUT__BT_TUT_01_HPP_
#define ROS2_BT_TUT__BT_TUT_01_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "behaviortree_cpp_v3/bt_factory.h"

using namespace BT;

// Example of a custom SyncActionNode (synchronous action)
// Always returns NodeStatus SUCCESS
class ApproachObject : public BT::SyncActionNode {
  public:
    // Constructor 
    ApproachObject(const std::string& name):BT::SyncActionNode(name, {}) {}

    // We must override the virtual function tick() since it contains the node's behavior and must return its execution status
    NodeStatus tick() override {
      std::cout << "ApproachObject: " << this->name() << std::endl;
      return NodeStatus::SUCCESS;
    }
};

// Condition node example where we can subscribe to a topic and check any data
// Always returns NodeStatus SUCCESS
NodeStatus CheckBattery() {
  std::cout << "Battery OK" << std::endl;
  return NodeStatus::SUCCESS;
}

// Class of Action nodes that sets Gripper _open status to true (open) or false (close)
// Always returns NodeStatus SUCCESS
class GripperInterface {
  public:
    // Constructor 
    GripperInterface():_open(true) {}

    NodeStatus open() {
      _open = true;
      std::cout << "Gripper open" << std::endl;
      // TODO: Try changing NodeStatus to FAILURE
      return NodeStatus::SUCCESS;
    }

    NodeStatus close() {
      _open = false;
      std::cout << "Gripper close" << std::endl;
      return NodeStatus::SUCCESS;
    }

  private:
    bool _open;
};

#endif //ROS2_BT_TUT__BT_TUT_01_HPP_