#ifndef ROS2_BT_TUT__BT_TUT_02_HPP_
#define ROS2_BT_TUT__BT_TUT_02_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "behaviortree_cpp_v3/action_node.h"
#include "behaviortree_cpp_v3/bt_factory.h"

using namespace BT;

// Example of a custom SyncActionNode (synchronous action)
// This action reads the input port "message" and prints/publishes it.
class SaySomething : public SyncActionNode {
  public:
    // Constructor 
    SaySomething(const std::string& name, const NodeConfiguration& config);
    
    // Static method
    static PortsList providedPorts();

    // We must override the virtual function tick()
    virtual NodeStatus tick() override;

  private:
  // We declare a shared node and publisher to share port data across nodes
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
};

// Another example of a custom SyncActionNode (synchronous action)
// This action writes a string value to the output port "text".
class ThinkWhatToSay : public SyncActionNode {
  public:
    // Contructor
    ThinkWhatToSay(const std::string& name, const NodeConfiguration& config);
    
    // Static method
    static PortsList providedPorts();

    // We must override the virtual function tick()
    virtual NodeStatus tick() override;
};

#endif //ROS2_BT_TUT__BT_TUT_02_HPP_