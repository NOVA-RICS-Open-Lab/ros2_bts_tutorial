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

// Custom type
struct Pose2D
{
  double x;
  double y;
  double theta;
};

// Convert "x;y;theta" string into Pose2D
namespace BT
{
template <>
inline Pose2D convertFromString(BT::StringView str)
{
  auto parts = BT::splitString(str, ';');
  if (parts.size() != 3)
  {
    throw BT::RuntimeError("invalid input for Pose2D, expected format: x;y;theta");
  }

  Pose2D output;
  output.x = BT::convertFromString<double>(parts[0]);
  output.y = BT::convertFromString<double>(parts[1]);
  output.theta = BT::convertFromString<double>(parts[2]);
  return output;
}
}  // namespace BT

namespace chr = std::chrono;

// -------------------------
// BatteryOK condition
// -------------------------
class BatteryOK : public BT::ConditionNode
{
public:
  BatteryOK(const std::string& name, const BT::NodeConfiguration& config)
  : BT::ConditionNode(name, config), tick_count_(0)
  {}

  static BT::PortsList providedPorts()
  {
    return {};
  }

  BT::NodeStatus tick() override
  {
    tick_count_++;

    if (tick_count_ == 1)
    {
      std::cout << "[ Battery: OK ]" << std::endl;
      return BT::NodeStatus::SUCCESS;
    }
    else
    {
      std::cout << "[ Battery: OK ]" << std::endl;
      return BT::NodeStatus::SUCCESS;
    }
  }

private:
  int tick_count_;
};

// -------------------------
// SaySomething sync action
// -------------------------
class SaySomething : public BT::SyncActionNode
{
public:
  SaySomething(const std::string& name, const BT::NodeConfiguration& config)
  : BT::SyncActionNode(name, config)
  {}

  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<std::string>("message") };
  }

  BT::NodeStatus tick() override
  {
    auto msg = getInput<std::string>("message");
    if (!msg)
    {
      throw BT::RuntimeError("missing required input [message]: ", msg.error());
    }

    std::cout << "Robot says: " << msg.value() << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};

// -------------------------
// MoveBase asynchronous action
// -------------------------
class MoveBaseAction : public BT::StatefulActionNode
{
public:
  MoveBaseAction(const std::string& name, const BT::NodeConfiguration& config)
  : BT::StatefulActionNode(name, config)
  {}

  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<Pose2D>("goal") };
  }

  BT::NodeStatus onStart() override
  {
    if (!getInput<Pose2D>("goal", goal_))
    {
      throw BT::RuntimeError("missing required input [goal]");
    }

    printf("[ MoveBase: SEND REQUEST ]. goal: x=%f y=%f theta=%f\n",
           goal_.x, goal_.y, goal_.theta);

    // Simulate action duration
    completion_time_ = chr::system_clock::now() + chr::milliseconds(500);
    return BT::NodeStatus::RUNNING;
  }

  BT::NodeStatus onRunning() override
  {
    std::this_thread::sleep_for(chr::milliseconds(10));

    if (chr::system_clock::now() >= completion_time_)
    {
      std::cout << "[ MoveBase: FINISHED ]" << std::endl;
      return BT::NodeStatus::SUCCESS;
    }

    return BT::NodeStatus::RUNNING;
  }

  void onHalted() override
  {
    std::cout << "[ MoveBase: ABORTED ]" << std::endl;
  }

private:
  Pose2D goal_;
  chr::system_clock::time_point completion_time_;
};

#endif  // ROS2_BT_TUT__BT_TUT_04_REACTIVE_SEQUENCE_HPP_