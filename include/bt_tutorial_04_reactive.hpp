#ifndef ROS2_BT_TUT__BT_TUT_04_REACTIVE_SEQUENCE_HPP_
#define ROS2_BT_TUT__BT_TUT_04_REACTIVE_SEQUENCE_HPP_

#include "rclcpp/rclcpp.hpp"

#include "behaviortree_cpp_v3/action_node.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "behaviortree_cpp_v3/condition_node.h"

#include <chrono>
#include <thread>
#include <string>
#include <iostream>

// Custom type used by MoveBase to represent a 2D navigation goal
struct Pose2D {
  double x;
  double y;
  double theta;
};

// Convert "x;y;theta" strings from XML into the Pose2D custom type
namespace BT {
  template <> inline
  Pose2D convertFromString(BT::StringView str) {
    auto parts = BT::splitString(str, ';');
    if (parts.size() != 3) {
      throw BT::RuntimeError("Invalid input for Pose2D, expected format: x;y;theta");
    }

    Pose2D output;
    output.x = BT::convertFromString<double>(parts[0]);
    output.y = BT::convertFromString<double>(parts[1]);
    output.theta = BT::convertFromString<double>(parts[2]);
    return output;
  }
}  // namespace BT

namespace chr = std::chrono;

// Example of a custom ConditionNode
// This condition checks the battery state and returns SUCCESS or FAILURE
class BatteryOK : public BT::ConditionNode {
  public:
    // Constructor
    BatteryOK(const std::string& name, const BT::NodeConfiguration& config)
    : BT::ConditionNode(name, config), tick_count_(0) {}

    // Static method
    static BT::PortsList providedPorts() {
      return {};
    }

    // The tick() of a ConditionNode checks a condition and returns SUCCESS or FAILURE
    BT::NodeStatus tick() override {
      tick_count_++;
      // TODO: Utilize the given tick count logic to alternate between returning SUCCESS and FAILURE
      if (tick_count_ == 1) {
        std::cout << "[ Battery: OK ]" << std::endl;
        return BT::NodeStatus::SUCCESS;
      }

      else {
        std::cout << "[ Battery: OK ]" << std::endl;
        return BT::NodeStatus::SUCCESS;
      }
    }

  private:
    int tick_count_;
};

// Example of a custom SyncActionNode (synchronous action)
// This action reads the input port "message" and prints it
class SaySomething : public BT::SyncActionNode {
  public:
    // Constructor
    SaySomething(const std::string& name, const BT::NodeConfiguration& config)
    : BT::SyncActionNode(name, config) {}

    // Static method
    static BT::PortsList providedPorts() {
      return { BT::InputPort<std::string>("message") };
    }

    // The tick() of a SyncActionNode executes the action and returns immediately
    BT::NodeStatus tick() override {
      auto msg = getInput<std::string>("message");
      if (!msg) {
        throw BT::RuntimeError("missing required input [message]: ", msg.error());
      }

      std::cout << "Robot says: " << msg.value() << std::endl;
      return BT::NodeStatus::SUCCESS;
    }
};

// Example of a custom StatefulActionNode (asynchronous action)
// This action receives a navigation goal and simulates motion over time
class MoveBaseAction : public BT::StatefulActionNode {
  public:
    // Constructor
    MoveBaseAction(const std::string& name, const BT::NodeConfiguration& config)
    : BT::StatefulActionNode(name, config) {}

    // Static method
    static BT::PortsList providedPorts() {
      return { BT::InputPort<Pose2D>("goal") };
    }

    // onStart() is called once when the asynchronous action begins execution
    BT::NodeStatus onStart() override {
      if (!getInput<Pose2D>("goal", goal_)) {
        throw BT::RuntimeError("missing required input [goal]");
      }

      printf("[ MoveBase: SEND REQUEST ]. goal: x=%f y=%f theta=%f\n", goal_.x, goal_.y, goal_.theta);

      // Simulate action duration
      completion_time_ = chr::system_clock::now() + chr::milliseconds(500);
      return BT::NodeStatus::RUNNING;
    }

    // onRunning() is called while the asynchronous action is still RUNNING
    BT::NodeStatus onRunning() override {
      std::this_thread::sleep_for(chr::milliseconds(10));

      if (chr::system_clock::now() >= completion_time_) {
        std::cout << "[ MoveBase: FINISHED ]" << std::endl;
        return BT::NodeStatus::SUCCESS;
      }

      return BT::NodeStatus::RUNNING;
    }

    // onHalted() is called if the action is interrupted before finishing
    void onHalted() override {
      std::cout << "[ MoveBase: ABORTED ]" << std::endl;
    }

  private:
    Pose2D goal_;
    chr::system_clock::time_point completion_time_;
};

#endif  // ROS2_BT_TUT__BT_TUT_04_REACTIVE_SEQUENCE_HPP_
