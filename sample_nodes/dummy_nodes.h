// ============================================================================
// sample_nodes/dummy_nodes.h
// ----------------------------------------------------------------------------
// 教程共用的“示例节点库”。教程 02 / 04 / 06 / 07 / 09 / 15 都会用到它。
// 这里集中演示了自定义 TreeNode 的几种写法：
//   1) 继承 BT::SyncActionNode  —— 同步动作（无端口）
//   2) 继承 BT::SyncActionNode  —— 带输入端口 message
//   3) 继承 BT::StatefulActionNode —— 异步动作（会返回 RUNNING）
//   4) 用普通函数/仿函数 + registerSimpleAction 注册
// ============================================================================
#ifndef SIMPLE_BT_NODES_H
#define SIMPLE_BT_NODES_H

#include <chrono>
#include <functional>
#include <iostream>
#include <string>

#include "behaviortree_cpp/behavior_tree.h"
#include "behaviortree_cpp/bt_factory.h"

namespace DummyNodes
{

using BT::NodeStatus;

NodeStatus CheckBattery();
NodeStatus CheckTemperature();
NodeStatus SayHello();

class GripperInterface
{
public:
  GripperInterface() = default;

  NodeStatus open();
  NodeStatus close();

private:
  bool _opened = true;
};

//--------------------------------------
// 1) 继承 BT::SyncActionNode 的同步动作，没有端口。
class ApproachObject : public BT::SyncActionNode
{
public:
  ApproachObject(const std::string& name) : BT::SyncActionNode(name, {})
  {}

  // 必须重写虚函数 tick()
  NodeStatus tick() override;
};

// 2) 同样是同步动作，但多了一个输入端口 "message"。
class SaySomething : public BT::SyncActionNode
{
public:
  SaySomething(const std::string& name, const BT::NodeConfig& config)
    : BT::SyncActionNode(name, config)
  {}

  NodeStatus tick() override;

  // 带端口的节点必须实现这个静态方法，声明自己有哪些端口。
  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<std::string>("message") };
  }
};

// 与 SaySomething 功能相同，但会被当成 SimpleActionNode 注册（见 .cpp）。
NodeStatus SaySomethingSimple(BT::TreeNode& self);

// 3) 异步节点示例：继承 BT::StatefulActionNode。
//    它会先返回 RUNNING，等到时间到了再返回 SUCCESS。
class SleepNode : public BT::StatefulActionNode
{
public:
  SleepNode(const std::string& name, const BT::NodeConfig& config)
    : BT::StatefulActionNode(name, config)
  {}

  static BT::PortsList providedPorts()
  {
    // 需要睡眠的毫秒数
    return { BT::InputPort<int>("msec") };
  }

  NodeStatus onStart() override
  {
    int msec = 0;
    getInput("msec", msec);
    if(msec <= 0)
    {
      // 不需要进入 RUNNING 状态
      return NodeStatus::SUCCESS;
    }
    else
    {
      using namespace std::chrono;
      // 到达该时刻后就返回 SUCCESS
      deadline_ = system_clock::now() + milliseconds(msec);
      return NodeStatus::RUNNING;
    }
  }

  // 只要还处于 RUNNING，就会被反复调用
  NodeStatus onRunning() override
  {
    if(std::chrono::system_clock::now() >= deadline_)
    {
      return NodeStatus::SUCCESS;
    }
    else
    {
      return NodeStatus::RUNNING;
    }
  }

  void onHalted() override
  {
    // 被其它节点打断时调用，这里什么也不做
    std::cout << "SleepNode interrupted" << std::endl;
  }

private:
  std::chrono::system_clock::time_point deadline_;
};

// 一次性把上面这些节点全部注册进 factory（供 .cpp 里的插件入口调用）
inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
{
  static GripperInterface grip_singleton;

  factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));
  factory.registerSimpleCondition("CheckTemperature", std::bind(CheckTemperature));
  factory.registerSimpleAction("SayHello", std::bind(SayHello));
  factory.registerSimpleAction("OpenGripper",
                               std::bind(&GripperInterface::open, &grip_singleton));
  factory.registerSimpleAction("CloseGripper",
                               std::bind(&GripperInterface::close, &grip_singleton));
  factory.registerNodeType<ApproachObject>("ApproachObject");
  factory.registerNodeType<SaySomething>("SaySomething");
}

}  // namespace DummyNodes

#endif  // SIMPLE_BT_NODES_H
