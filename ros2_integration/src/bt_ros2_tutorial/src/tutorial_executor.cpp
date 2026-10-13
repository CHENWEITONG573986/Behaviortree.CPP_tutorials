// ============================================================================
// tutorial_executor.cpp
// ----------------------------------------------------------------------------
// 行为树执行器（Task Planner 的最小实现）。
//
// 它做四件事：
//   1) 创建一个 ROS 节点（行为树里的 ROS 节点都靠它来通信）；
//   2) 把自定义节点注册进 BehaviorTreeFactory；
//      —— 注册 ROS 节点时，要通过 BT::RosNodeParams 把 ROS 节点句柄传进去；
//   3) 从 XML 文件创建行为树；
//   4) tick 它，直到跑完。
//
// 特别说明（初学者常问）：
//   RosActionNode / RosServiceNode 的 tick() 内部会自己处理 ROS 回调
//   （它们各自有自己的 callback group 和 executor），所以这里**不需要**再
//   手动调用 executor.spin()。直接 tree.tickWhileRunning() 即可。
// ============================================================================
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>

#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_ros2/ros_node_params.hpp"

#include "ament_index_cpp/get_package_share_directory.hpp"

#include "bt_ros2_tutorial/countdown_action.hpp"
#include "bt_ros2_tutorial/add_two_ints_service.hpp"
#include "bt_ros2_tutorial/print_nodes.hpp"

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);

  // 1) 行为树的 ROS 节点。注意：这不是 TreeNode，而是普通的 rclcpp::Node，
  //    所有 RosXxxNode 都通过它来收发消息。
  auto node = std::make_shared<rclcpp::Node>("bt_tutorial_executor");

  BT::BehaviorTreeFactory factory;

  // 2a) 注册两个“普通”节点（不涉及 ROS）
  factory.registerNodeType<bt_ros2_tutorial::PrintValue>("PrintValue");
  factory.registerNodeType<bt_ros2_tutorial::PrintNumber>("PrintNumber");

  // 2b) 注册 ROS Action 包装节点。
  //     RosNodeParams：
  //       nh                 —— 上面那个 ROS 节点
  //       default_port_value —— 默认的“动作服务名”，会被用作 action_name 端口
  //                             （XML 里没写 action_name 时就用它）
  BT::RosNodeParams action_params;
  action_params.nh = node;
  action_params.default_port_value = "countdown";
  factory.registerNodeType<CountdownAction>("Countdown", action_params);

  // 2c) 注册 ROS Service 包装节点，服务名默认 "add_two_ints"
  BT::RosNodeParams service_params;
  service_params.nh = node;
  service_params.default_port_value = "add_two_ints";
  factory.registerNodeType<AddTwoIntsService>("AddTwoInts", service_params);

  // 3) 从 XML 创建行为树。XML 装在包的 share 目录下，用 ament 索引定位。
  const std::string share = ament_index_cpp::get_package_share_directory("bt_ros2_tutorial");
  const std::string xml_path = share + "/behavior_trees/countdown_tree.xml";
  RCLCPP_INFO(node->get_logger(), "加载行为树文件: %s", xml_path.c_str());

  auto tree = factory.createTreeFromFile(xml_path);

  // 4) tick 整棵树。
  //    tickWhileRunning() 会反复 tick，直到根节点返回 SUCCESS 或 FAILURE。
  RCLCPP_INFO(node->get_logger(), "开始执行行为树 ...");
  const BT::NodeStatus status = tree.tickWhileRunning();
  RCLCPP_INFO(node->get_logger(), "行为树执行结束，最终状态: %s",
              BT::toStr(status).c_str());

  rclcpp::shutdown();
  return 0;
}
