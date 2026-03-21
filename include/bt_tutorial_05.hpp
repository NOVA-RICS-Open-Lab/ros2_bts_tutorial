#ifndef ROS2_BT_TUT__BT_TUT_05_HPP_
#define ROS2_BT_TUT__BT_TUT_05_HPP_

#include "behaviortree_cpp_v3/bt_factory.h"
#include <memory>
#include <string>

struct DoorExecutionContext {
  std::string decoded_code;
  std::string door_result;
};

class CrossDoor {
  public:
    explicit CrossDoor(std::shared_ptr<DoorExecutionContext> context = nullptr);

    // Register all the simple nodes associated with the door interaction example
    void registerNodes(BT::BehaviorTreeFactory& factory);

    // Condition node logic: SUCCESS if the door is closed
    BT::NodeStatus isDoorClosed();

    // Action node logic: SUCCESS if we can pass through an open door
    BT::NodeStatus passThroughDoor();

    // Action node logic: simulates trying to pick the lock
    BT::NodeStatus pickLock();

    // Action node logic: tries to open the door normally, fails if it is locked
    BT::NodeStatus openDoor();

    // Action node logic: always forces the door open
    BT::NodeStatus smashDoor();

  private:
    std::shared_ptr<DoorExecutionContext> context_;
    bool _door_open = false;
    bool _door_locked = true;
    int _pick_attempts = 0;
};

// Example of a custom SyncActionNode (synchronous action)
// This action simulates decoding a door access code and writes it to shared state
class DecodeDoorCode : public BT::SyncActionNode {
  public:
    // Constructor
    DecodeDoorCode(
      const std::string& name,
      const BT::NodeConfiguration& config,
      std::shared_ptr<DoorExecutionContext> context);

    // Static method
    static BT::PortsList providedPorts();

    // The tick() of a SyncActionNode executes the action and returns immediately
    BT::NodeStatus tick() override;

  private:
    std::shared_ptr<DoorExecutionContext> context_;
};

// Example of a custom SyncActionNode (synchronous action)
// This action reads a decoded code from shared state and tries to use it
class UseDoorCode : public BT::SyncActionNode {
  public:
    // Constructor
    UseDoorCode(
      const std::string& name,
      const BT::NodeConfiguration& config,
      CrossDoor& cross_door,
      std::shared_ptr<DoorExecutionContext> context);

    // Static method
    static BT::PortsList providedPorts();

    // The tick() of a SyncActionNode executes the action and returns immediately
    BT::NodeStatus tick() override;

  private:
    CrossDoor& cross_door_;
    std::shared_ptr<DoorExecutionContext> context_;
};

// Example of a custom SyncActionNode (synchronous action)
// This action reads a text value from shared state and logs the final result
class LogResult : public BT::SyncActionNode {
  public:
    // Constructor
    LogResult(
      const std::string& name,
      const BT::NodeConfiguration& config,
      std::shared_ptr<DoorExecutionContext> context);

    // Static method
    static BT::PortsList providedPorts();

    // The tick() of a SyncActionNode executes the action and returns immediately
    BT::NodeStatus tick() override;

  private:
    std::shared_ptr<DoorExecutionContext> context_;
};

// Helper function that registers the additional nodes used by the password-based subtree flow
void registerPasswordNodes(
  BT::BehaviorTreeFactory& factory,
  CrossDoor& cross_door,
  std::shared_ptr<DoorExecutionContext> context
);

#endif // ROS2_BT_TUT__BT_TUT_05_HPP_
