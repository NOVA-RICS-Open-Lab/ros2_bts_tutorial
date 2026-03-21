#include "rclcpp/rclcpp.hpp"
#include "bt_tutorial_02.hpp"
#include "bt_tutorial_05.hpp"
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

using namespace BT;

// Constructor
SaySomething::SaySomething(
  const std::string& name,
  const NodeConfiguration& config
):SyncActionNode(name, config) {
  node_ = rclcpp::Node::make_shared("input");
  pub_ = node_->create_publisher<std_msgs::msg::String>("/input_string", 10);
}

// It is mandatory to define this STATIC method.
PortsList SaySomething::providedPorts() {
  // This action has a single input port called "message"
  return { InputPort<std::string>("message") };
}

// Override the virtual function tick()
NodeStatus SaySomething::tick() {
  Optional<std::string> msg = getInput<std::string>("message");

  // Check if optional is valid. If not, throw its error
  if (!msg) {
    throw RuntimeError("missing required input [message]: ", msg.error());
  }

  // Use the method value() to extract the valid message.
  auto msg_data = msg.value();
  std::cout << "Robot says: " << msg_data << std::endl;

  std_msgs::msg::String input_msg;
  input_msg.data = msg_data;
  RCLCPP_INFO(node_->get_logger(), "input data: %s", input_msg.data.c_str());
  pub_->publish(input_msg);

  return NodeStatus::SUCCESS;
}

// Constructor
ThinkWhatToSay::ThinkWhatToSay(
  const std::string& name,
  const NodeConfiguration& config
):SyncActionNode(name, config) {}

// It is mandatory to define this STATIC method.
PortsList ThinkWhatToSay::providedPorts() {
  return { OutputPort<std::string>("text") };
}

// This action writes a value into the port "text"
NodeStatus ThinkWhatToSay::tick() {
  // The output may change at each tick(). Here we keep it simple.
  setOutput("text", std::string("The answer is 42"));
  return NodeStatus::SUCCESS;
}

// Small helper used to simulate action execution time.
inline void SleepMS(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

CrossDoor::CrossDoor(
  std::shared_ptr<DoorExecutionContext> context
):context_(std::move(context)) {}

NodeStatus CrossDoor::isDoorClosed() {
  SleepMS(500);

  if (!_door_open) {
    std::cout << "isDoorClosed: SUCCESS" << std::endl;
  
  } else {
    std::cout << "isDoorClosed: FAILURE" << std::endl;
  }

  return !_door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus CrossDoor::passThroughDoor() {
  SleepMS(500);

  if (_door_open) {
    std::cout << "passThroughDoor: SUCCESS" << std::endl;
  
  } else {
    std::cout << "passThroughDoor: FAILURE" << std::endl;
  }

  return _door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus CrossDoor::openDoor() {
  SleepMS(500);

  if (_door_locked) {
    std::cout << "openDoor: FAILURE" << std::endl;
    return NodeStatus::FAILURE;
  }

  _door_open = true;
  if (context_) {
    context_->door_result = "OpenDoor: SUCCESS";
  }

  std::cout << "openDoor: SUCCESS" << std::endl;
  return NodeStatus::SUCCESS;
}

NodeStatus CrossDoor::pickLock() {
  SleepMS(500);
  _pick_attempts++;

  // Succeed at the 3rd attempt.
  if (_door_locked && _pick_attempts < 3) {
    std::cout << "pick a Lock: FAILURE" << std::endl;
    return NodeStatus::FAILURE;
  }

  _door_locked = false;
  _door_open = true;
  if (context_) {
    context_->door_result = "PickLock: SUCCESS";
  }

  std::cout << "pick a Lock: SUCCESS" << std::endl;
  return NodeStatus::SUCCESS;
}

NodeStatus CrossDoor::smashDoor() {
  _door_locked = false;
  _door_open = true;
  if (context_) {
    context_->door_result = "SmashDoor: SUCCESS";
  }

  // Smash always works.
  std::cout << "smashDoor: SUCCESS" << std::endl;
  return NodeStatus::SUCCESS;
}

void CrossDoor::registerNodes(BT::BehaviorTreeFactory& factory) {
  factory.registerSimpleCondition(
    "IsDoorClosed",
    std::bind(&CrossDoor::isDoorClosed, this)
  );

  factory.registerSimpleAction(
    "PassThroughDoor",
    std::bind(&CrossDoor::passThroughDoor, this)
  );

  factory.registerSimpleAction(
    "OpenDoor",
    std::bind(&CrossDoor::openDoor, this)
  );

  factory.registerSimpleAction(
    "PickLock",
    std::bind(&CrossDoor::pickLock, this)
  );

  factory.registerSimpleAction(
    "SmashDoor",
    std::bind(&CrossDoor::smashDoor, this)
  );
}

// Constructor
DecodeDoorCode::DecodeDoorCode(
  const std::string& name,
  const BT::NodeConfiguration& config,
  std::shared_ptr<DoorExecutionContext> context
):BT::SyncActionNode(name, config), context_(std::move(context)) {}

// Static method
BT::PortsList DecodeDoorCode::providedPorts() {
  return {};
}

BT::NodeStatus DecodeDoorCode::tick() {
  if (!context_) {
    throw BT::RuntimeError("DecodeDoorCode has no shared execution context");
  }

  std::cout << "DecodeDoorCode: deciphering security code..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));

  context_->decoded_code = "A1B2";
  std::cout << "DecodeDoorCode: code ready" << std::endl;
  return BT::NodeStatus::SUCCESS;
}

// Constructor
UseDoorCode::UseDoorCode(
  const std::string& name,
  const BT::NodeConfiguration& config,
  CrossDoor& cross_door,
  std::shared_ptr<DoorExecutionContext> context
):BT::SyncActionNode(name, config),
  cross_door_(cross_door),
  context_(std::move(context)) {}

// Static method
BT::PortsList UseDoorCode::providedPorts() {
  return {};
}

BT::NodeStatus UseDoorCode::tick() {
  if (!context_) {
    throw BT::RuntimeError("UseDoorCode has no shared execution context");
  }

  if (context_->decoded_code == "A1B2") {
    auto status = cross_door_.smashDoor();
    if (status == BT::NodeStatus::SUCCESS) {
      context_->door_result = "DoorCode: SUCCESS";
    }

    std::cout << "UseDoorCode: SUCCESS" << std::endl;
    return status;
  }

  std::cout << "UseDoorCode: FAILURE" << std::endl;
  return BT::NodeStatus::FAILURE;
}

// Constructor
LogResult::LogResult(
  const std::string& name,
  const BT::NodeConfiguration& config,
  std::shared_ptr<DoorExecutionContext> context
):BT::SyncActionNode(name, config), context_(std::move(context)) {}

// Static method
BT::PortsList LogResult::providedPorts() {
  return {};
}

BT::NodeStatus LogResult::tick() {
  if (!context_) {
    throw BT::RuntimeError("LogResult has no shared execution context");
  }

  std::cout << "MainTree result: " << context_->door_result << std::endl;
  return BT::NodeStatus::SUCCESS;
}

void registerPasswordNodes(
  BT::BehaviorTreeFactory& factory,
  CrossDoor& cross_door,
  std::shared_ptr<DoorExecutionContext> context
) {
  BT::NodeBuilder decode_door_code_builder = [context](
    const std::string& name,
    const BT::NodeConfiguration& config
  ) {
    return std::make_unique<DecodeDoorCode>(name, config, context);
  };

  factory.registerBuilder<DecodeDoorCode>("DecodeDoorCode", decode_door_code_builder);

  BT::NodeBuilder use_door_code_builder = [&cross_door, context](
    const std::string& name,
    const BT::NodeConfiguration& config
  ) {
    return std::make_unique<UseDoorCode>(name, config, cross_door, context);
  };

  factory.registerBuilder<UseDoorCode>("UseDoorCode", use_door_code_builder);

  BT::NodeBuilder log_result_builder = [context](
    const std::string& name,
    const BT::NodeConfiguration& config
  ) {
    return std::make_unique<LogResult>(name, config, context);
  };
  
  factory.registerBuilder<LogResult>("LogResult", log_result_builder);
}
