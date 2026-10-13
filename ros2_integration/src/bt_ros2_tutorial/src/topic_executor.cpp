// ============================================================================
// topic_executor.cpp
// ----------------------------------------------------------------------------
// Topic 示例的执行器。和 countdown 那个执行器相比，多了一个“等待外部事件”的技巧。
//
// 为什么不能简单 tick 一次就完事？
//   订阅节点(CheckTemperature)要等到 DDS 真正把消息送到、并且订阅端完成“发现”
//   之后，才能判定成功。刚启动时可能还没有消息，第一次 tick 会 FAILURE。
//
// 解决办法：反复 tickOnce()，直到成功或超时。
//   可行为什么？因为 Tree 在每次 tick 结束（返回 SUCCESS/FAILURE）后会自动
//   resetStatus()，所以下一次 tick 会从根节点重新开始执行整棵树。
//   本树把“等待温度”放在最前面，成功后才继续往下去发布指令，
//   因此发布指令只会执行一次。
// ============================================================================
#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include <rclcpp/rclcpp.hpp>

#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_ros2/ros_node_params.hpp"

#include "ament_index_cpp/get_package_share_directory.hpp"

#include "bt_ros2_tutorial/topic_nodes.hpp"
#include "bt_ros2_tutorial/print_nodes.hpp"

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("bt_topic_executor");
  BT::BehaviorTreeFactory factory;

  // 普通节点
  factory.registerNodeType<bt_ros2_tutorial::PrintValue>("PrintValue");

  // 订阅节点：默认话题 /temperature
  BT::RosNodeParams sub_params;
  sub_params.nh = node;
  sub_params.default_port_value = "/temperature";
  factory.registerNodeType<CheckTemperature>("CheckTemperature", sub_params);

  // 发布节点：默认话题 /robot_command
  BT::RosNodeParams pub_params;
  pub_params.nh = node;
  pub_params.default_port_value = "/robot_command";
  factory.registerNodeType<PublishCommand>("PublishCommand", pub_params);

  const std::string share = ament_index_cpp::get_package_share_directory("bt_ros2_tutorial");
  const std::string xml_path = share + "/behavior_trees/topic_tree.xml";
  RCLCPP_INFO(node->get_logger(), "加载行为树文件: %s", xml_path.c_str());

  auto tree = factory.createTreeFromFile(xml_path);

  // 反复 tick，直到成功或超时（最多等 10 秒）
  RCLCPP_INFO(node->get_logger(), "开始执行（会等待温度话题的消息）...");
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
  BT::NodeStatus status = BT::NodeStatus::FAILURE;
  int attempts = 0;
  while(std::chrono::steady_clock::now() < deadline)
  {
    ++attempts;
    status = tree.tickOnce();
    if(status == BT::NodeStatus::SUCCESS)
    {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  RCLCPP_INFO(node->get_logger(), "执行结束（tick 了 %d 次），最终状态: %s", attempts,
              BT::toStr(status).c_str());

  rclcpp::shutdown();
  return 0;
}
