#include "rclcpp/rclcpp.hpp"
#include "bt_tutorial_04_sequence.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"

#include <cstdlib>
#include <string>
#include <memory>
#include <iostream>

#define GROOT_CMD "groot2"

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  const std::string bt_xml = ament_index_cpp::get_package_share_directory("ros2_bts_tutorial") + "/config/bt_tutorial_04_sequence_tree.xml";

  BT::BehaviorTreeFactory factory;

  factory.registerNodeType<BatteryOK>("BatteryOK");
  factory.registerNodeType<SaySomething>("SaySomething");
  factory.registerNodeType<MoveBaseAction>("MoveBase");

  // Launch Groot in background
  std::system((std::string(GROOT_CMD) + " --file " + bt_xml + " &").c_str());

  auto tree = factory.createTreeFromFile(bt_xml);

  BT::NodeStatus status = BT::NodeStatus::IDLE;
  while (rclcpp::ok())
  {
    std::cout << "--- ticking" << std::endl;
    status = tree.tickRoot();
    std::cout << "--- status: " << BT::toStr(status) << "\n" << std::endl;

    if (status == BT::NodeStatus::SUCCESS)
    {
      break;
    }

    tree.sleep(std::chrono::milliseconds(100));
  }

  rclcpp::shutdown();
  return 0;
}
