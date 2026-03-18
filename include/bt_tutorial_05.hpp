#ifndef ROS2_BT_TUT__BT_TUT_05_HPP_
#define ROS2_BT_TUT__BT_TUT_05_HPP_

#include "behaviortree_cpp_v3/bt_factory.h"

class CrossDoor
{
public:
  void registerNodes(BT::BehaviorTreeFactory& factory);

  // SUCCESS if _door_open == false
  BT::NodeStatus isDoorClosed();

  // SUCCESS if _door_open == true
  BT::NodeStatus passThroughDoor();

  // After 3 attempts, will open a locked door
  BT::NodeStatus pickLock();

  // FAILURE if door is locked
  BT::NodeStatus openDoor();

  // Always opens the door
  BT::NodeStatus smashDoor();

private:
  bool _door_open = false;
  bool _door_locked = true;
  int _pick_attempts = 0;
};

// Help for Exercise 5
class DecodeDoorCode : public BT::SyncActionNode
{
public:
  DecodeDoorCode(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
};

class UseDoorCode : public BT::SyncActionNode
{
public:
  UseDoorCode(const std::string& name, const BT::NodeConfiguration& config, CrossDoor& cross_door);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;

private:
  CrossDoor& cross_door_;
};

class LogResult : public BT::SyncActionNode
{
public:
  LogResult(const std::string& name, const BT::NodeConfiguration& config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;
};

void registerPasswordNodes(BT::BehaviorTreeFactory& factory, CrossDoor& cross_door);

#endif // ROS2_BT_TUT__BT_TUT_05_HPP_
