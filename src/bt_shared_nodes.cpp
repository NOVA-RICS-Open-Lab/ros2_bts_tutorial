#include "rclcpp/rclcpp.hpp"
#include "bt_tutorial_02.hpp"
#include "bt_tutorial_05.hpp"
#include "bt_tutorial_05_solution.hpp"
#include <chrono>
#include <functional>
#include <memory>
#include <thread>

using namespace BT;

// Constructor 
SaySomething::SaySomething(const std::string& name, const NodeConfiguration& config)
    : SyncActionNode(name, config)
{
  node_ = rclcpp::Node::make_shared("input");
  pub_ = node_->create_publisher<std_msgs::msg::String>("/input_string",10);
}

// It is mandatory to define this STATIC method.
PortsList SaySomething::providedPorts()
{
  // This action has a single input port called "message"
  return { InputPort<std::string>("message") };
}

// Override the virtual function tick()
NodeStatus SaySomething::tick()
{
  Optional<std::string> msg = getInput<std::string>("message");
  // Check if optional is valid. If not, throw its error
  if (!msg)
  {
    throw RuntimeError("missing required input [message]: ", msg.error() );
  }
  auto msg_data = msg.value();
  // use the method value() to extract the valid message.
  std::cout << "Robot says: " << msg_data << std::endl;

  std_msgs::msg::String input_msg;
  input_msg.data = msg_data;
  RCLCPP_INFO(node_->get_logger(), "input data: %s", input_msg.data.c_str());
  pub_->publish(input_msg);

  return NodeStatus::SUCCESS;
}


ThinkWhatToSay::ThinkWhatToSay(const std::string& name, const NodeConfiguration& config)
  : SyncActionNode(name, config)
{ }

PortsList ThinkWhatToSay::providedPorts()
{
  return { OutputPort<std::string>("text") };
}

// This Action writes a value into the port "text"
NodeStatus ThinkWhatToSay::tick()
{
  // the output may change at each tick(). Here we keep it simple.
  setOutput("text", std::string("The answer is 42"));
  return NodeStatus::SUCCESS;
}

inline void SleepMS(int ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

NodeStatus CrossDoor::isDoorClosed()
{
  SleepMS(500);
  if (_door_open)
  {
    std::cout << "isDoorClosed: SUCCESS" << std::endl;
  }
  else
  {
    std::cout << "isDoorClosed: FAILURE" << std::endl;
  }
  return !_door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus CrossDoor::passThroughDoor()
{
  SleepMS(500);
  if (_door_open)
  {
    std::cout << "passThroughDoor: SUCCESS" << std::endl;
  }
  else
  {
    std::cout << "passThroughDoor: FAILURE" << std::endl;
  }
  return _door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus CrossDoor::openDoor()
{
  SleepMS(500);
  if (_door_locked)
  {
    std::cout << "openDoor: FAILURE" << std::endl;
    return NodeStatus::FAILURE;
  }

  _door_open = true;
  std::cout << "openDoor: SUCCESS" << std::endl;
  return NodeStatus::SUCCESS;
}

NodeStatus CrossDoor::pickLock()
{
  SleepMS(500);
  _pick_attempts++;
  // succeed at 3rd attempt
  if (_door_locked && _pick_attempts < 3)
  {
    _door_locked = false;
    _door_open = true;
    std::cout << "pick a Lock: FAILURE" << std::endl;
    return NodeStatus::FAILURE;
  }
  std::cout << "pick a Lock: SUCCESS" << std::endl;
  return NodeStatus::SUCCESS;
}

NodeStatus CrossDoor::smashDoor()
{
  _door_locked = false;
  _door_open = true;
  // smash always works
  std::cout << "smashDoor: SUCCESS" << std::endl;
  return NodeStatus::SUCCESS;
}

void CrossDoor::registerNodes(BT::BehaviorTreeFactory &factory)
{
  factory.registerSimpleCondition(
      "IsDoorClosed", std::bind(&CrossDoor::isDoorClosed, this));

  factory.registerSimpleAction(
      "PassThroughDoor", std::bind(&CrossDoor::passThroughDoor, this));

  factory.registerSimpleAction(
      "OpenDoor", std::bind(&CrossDoor::openDoor, this));

  factory.registerSimpleAction(
      "PickLock", std::bind(&CrossDoor::pickLock, this));

  factory.registerSimpleCondition(
      "SmashDoor", std::bind(&CrossDoor::smashDoor, this));
}

DecodeDoorCode::DecodeDoorCode(const std::string& name, const BT::NodeConfiguration& config)
: BT::SyncActionNode(name, config)
{}

BT::PortsList DecodeDoorCode::providedPorts()
{
  return { BT::OutputPort<std::string>("code") };
}

BT::NodeStatus DecodeDoorCode::tick()
{
  std::cout << "DecodeDoorCode: deciphering security code..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));

  setOutput("code", std::string("A1B2"));
  std::cout << "DecodeDoorCode: code ready" << std::endl;
  return BT::NodeStatus::SUCCESS;
}

UseDoorCode::UseDoorCode(const std::string& name, const BT::NodeConfiguration& config, CrossDoor& cross_door)
: BT::SyncActionNode(name, config), cross_door_(cross_door)
{}

BT::PortsList UseDoorCode::providedPorts()
{
  return { BT::InputPort<std::string>("code") };
}

BT::NodeStatus UseDoorCode::tick()
{
  auto code = getInput<std::string>("code");
  if (!code)
  {
    throw BT::RuntimeError("missing required input [code]: ", code.error());
  }

  if (code.value() == "A1B2")
  {
    std::cout << "UseDoorCode: SUCCESS" << std::endl;
    return cross_door_.smashDoor();
  }

  std::cout << "UseDoorCode: FAILURE" << std::endl;
  return BT::NodeStatus::FAILURE;
}

LogResult::LogResult(const std::string& name, const BT::NodeConfiguration& config)
: BT::SyncActionNode(name, config)
{}

BT::PortsList LogResult::providedPorts()
{
  return { BT::InputPort<std::string>("text") };
}

BT::NodeStatus LogResult::tick()
{
  auto text = getInput<std::string>("text");
  if (!text)
  {
    throw BT::RuntimeError("missing required input [text]: ", text.error());
  }

  std::cout << "MainTree result: " << text.value() << std::endl;
  return BT::NodeStatus::SUCCESS;
}

void registerPasswordNodes(BT::BehaviorTreeFactory& factory, CrossDoor& cross_door)
{
  factory.registerNodeType<DecodeDoorCode>("DecodeDoorCode");
  factory.registerNodeType<LogResult>("LogResult");

  BT::NodeBuilder use_door_code_builder = [&cross_door](
      const std::string& name, const BT::NodeConfiguration& config) {
    return std::make_unique<UseDoorCode>(name, config, cross_door);
  };

  factory.registerBuilder<UseDoorCode>("UseDoorCode", use_door_code_builder);
}
