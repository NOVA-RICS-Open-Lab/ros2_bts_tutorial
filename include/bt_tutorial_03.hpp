#ifndef ROS2_BT_TUT__BT_TUT_03_STARTER_HPP_
#define ROS2_BT_TUT__BT_TUT_03_STARTER_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "behaviortree_cpp_v3/bt_factory.h"
// We might need to include some headers here

struct Position3D {
  double x;
  double y;
  double z;
};

// TODO: Since we are receiving arguments as numeric elements with might not need this
namespace BT {
  template <> inline
  Position3D convertFromString(StringView str) {
    auto parts = BT::splitString(str, ';');
    if (parts.size() != 3) {
      throw BT::RuntimeError("invalid input");
    }

    Position3D output;
    output.x = convertFromString<double>(parts[0]);
    output.y = convertFromString<double>(parts[1]);
    output.z = convertFromString<double>(parts[2]);
    return output;
  }
} // namespace BT

// PrintTarget simply prints target coordinates and returns SUCCESS NodeStatus
// TODO: Remake PrintTarget so it properly receives inputs as arguments instead of getting them from reading the Blackboard
class PrintTarget : public BT::SyncActionNode {
  public:
    // Contructor
    PrintTarget(
      const std::string& name,
      const BT::NodeConfiguration& config
      // TODO: Add taerget goal as a shared pointer
      // std::shared_ptr<TODO> target
    ):BT::SyncActionNode(name, config /*target_(std::move(TODO)*/) {}

    // TODO: To remove. We don't need to read from the Blackboard
    static BT::PortsList providedPorts() {
      const char* description = "Print target coordinates";
      return { BT::InputPort<Position3D>("target", description) };
    }

    // TODO: To simplify. We don't need to read from the Blackboard
    BT::NodeStatus tick() override {
      // TODO: To remove and replace with an if condition to check if target is set, if not throw an error
      auto res = getInput<Position3D>("target");
      if (!res) {
        throw BT::RuntimeError("Error reading port [target]: ", res.error());
      }

      // TODO: We no longer get res from reading Balckboard input port
      const Position3D target = res.value();
      printf("Target: [ %.1f, %.1f, %.1f ]\n", target.x, target.y, target.z);
      return BT::NodeStatus::SUCCESS;
    }

  //TODO: Add target shared argument as private for function internal usage
  /*
  private:
    std::shared_ptr<TODO> target_;
  */
};

// The CalculateGoal action should swap x and y values between each other making it the goal
// TODO: Create the CalculateGoal node here
/*
class CalculateGoal : public BT::SyncActionNode {
  public:
    CalculateGoal(
      const std::string& name,
      const BT::NodeConfiguration& config,
      double input_x,
      double input_y,
      double input_z,
      std::shared_ptr<TODO> goal
    ):BT::SyncActionNode(name, config),
      input_x_(input_x),
      input_y_(input_y),
      input_z_(input_z),
      goal_(std::move(TODO)) {}

    BT::NodeStatus tick() override {
      // TODO: Add a condition to check if goal is set, if not throw an error
      if (TODO) {
        TODO
      }
      
      // Goal pointer should be updated with the new swapped coordinates
      *goal_ = TODO;
      return BT::NodeStatus::SUCCESS;
    }

  private:
    double input_x_;
    double input_y_;
    double input_z_;
    std::shared_ptr<TODO> goal_;
};
*/

#endif // ROS2_BT_TUT__BT_TUT_03_STARTER_HPP_
