// ============================================================================
// print_nodes.hpp
// ----------------------------------------------------------------------------
// 两个“普通”的行为树节点（完全不涉及 ROS 通信）。
// 放它们的目的：
//   1) 打印一些文字，让行为树的执行顺序一目了然；
//   2) 说明普通节点和 ROS 节点可以混在同一棵树里。
//
// 这也是最简单的自定义节点写法：继承 BT::SyncActionNode，重写 tick()。
// ============================================================================
#pragma once

#include <behaviortree_cpp/action_node.h>

#include <cstdint>
#include <iostream>
#include <string>

namespace bt_ros2_tutorial
{

// ----------------------------------------------------------------------------
// PrintValue：把输入端口 message 的内容打印出来。
// ----------------------------------------------------------------------------
class PrintValue : public BT::SyncActionNode
{
public:
  // 带端口的节点，构造函数都必须长这样：(名字, 配置)。
  PrintValue(const std::string& name, const BT::NodeConfig& config)
    : BT::SyncActionNode(name, config)
  {}

  // 同步动作：tick() 里一次把活干完，返回 SUCCESS / FAILURE。
  BT::NodeStatus tick() override
  {
    std::string msg;
    if(!getInput("message", msg))
    {
      // 没拿到输入端口（XML 没写、类型不对）就返回失败
      return BT::NodeStatus::FAILURE;
    }
    std::cout << "[PrintValue] " << msg << std::endl;
    return BT::NodeStatus::SUCCESS;
  }

  // 声明本节点有哪些端口。普通字符串输入端口。
  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<std::string>("message", "要打印的文字") };
  }
};

// ----------------------------------------------------------------------------
// PrintNumber：打印一个整数，用来显示 AddTwoInts 输出端口 sum 的值。
//
// 注意端口类型用 int64_t，和 example_interfaces/AddTwoInts 的字段一致：
// 黑板是强类型的，如果一边按 int64 存、一边按 int 读，会因类型不匹配而失败。
// ----------------------------------------------------------------------------
class PrintNumber : public BT::SyncActionNode
{
public:
  PrintNumber(const std::string& name, const BT::NodeConfig& config)
    : BT::SyncActionNode(name, config)
  {}

  BT::NodeStatus tick() override
  {
    int64_t value = 0;
    if(!getInput("val", value))
    {
      return BT::NodeStatus::FAILURE;
    }
    std::cout << "[PrintNumber] " << value << std::endl;
    return BT::NodeStatus::SUCCESS;
  }

  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<int64_t>("val", "要打印的整数") };
  }
};

}  // namespace bt_ros2_tutorial
