#ifndef ROS2_BT_TUT__BT_TUT_03_STARTER_HPP_
#define ROS2_BT_TUT__BT_TUT_03_STARTER_HPP_

#include "behaviortree_cpp_v3/action_node.h"
#include "behaviortree_cpp_v3/bt_factory.h"

struct Position3D
{
  double x;
  double y;
  double z;
};

// Parse "x;y;z" strings from XML/blackboard into Position3D.
namespace BT
{
template <> inline
Position3D convertFromString(StringView str)
{
  auto parts = BT::splitString(str, ';');
  if (parts.size() != 3)
  {
    throw BT::RuntimeError("invalid input");
  }

  Position3D output;
  output.x = convertFromString<double>(parts[0]);
  output.y = convertFromString<double>(parts[1]);
  output.z = convertFromString<double>(parts[2]);
  return output;
}
} // namespace BT

class PrintTarget : public BT::SyncActionNode
{
public:
  PrintTarget(const std::string& name, const BT::NodeConfiguration& config)
  : BT::SyncActionNode(name, config)
  {}

  static BT::PortsList providedPorts()
  {
    const char* description = "Print target coordinates";
    return { BT::InputPort<Position3D>("target", description) };
  }

  BT::NodeStatus tick() override
  {
    auto res = getInput<Position3D>("target");
    if (!res)
    {
      throw BT::RuntimeError("error reading port [target]: ", res.error());
    }

    const Position3D target = res.value();
    printf("Target: [ %.1f, %.1f, %.1f ]\n", target.x, target.y, target.z);
    return BT::NodeStatus::SUCCESS;
  }
};

#endif // ROS2_BT_TUT__BT_TUT_03_STARTER_HPP_
