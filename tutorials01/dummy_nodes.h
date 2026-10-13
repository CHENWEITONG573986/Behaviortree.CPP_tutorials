// ============================================================================
// 教程 01 的自定义节点（本节专用的最简版本）
// 官方文档里把这几个节点写在 dummy_nodes.h 里。
// ============================================================================
#pragma once

#include <iostream>
#include <string>

#include "behaviortree_cpp/action_node.h"

namespace DummyNodes
{

// 示例：自定义同步动作节点（无端口）。
// 这是最推荐的写自定义节点的方式：继承 + 重写 tick()。
class ApproachObject : public BT::SyncActionNode
{
public:
  ApproachObject(const std::string& name) : BT::SyncActionNode(name, {})
  {}

  // 必须重写这个虚函数
  BT::NodeStatus tick() override
  {
    std::cout << "ApproachObject: " << this->name() << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
};

using namespace BT;

// 一个返回 NodeStatus 的普通函数：稍后会被包装成条件节点
BT::NodeStatus CheckBattery()
{
  std::cout << "[ Battery: OK ]" << std::endl;
  return BT::NodeStatus::SUCCESS;
}

// 把类的 open() / close() 包装成动作节点。
// 注意 _open 是节点之间共享的状态。
class GripperInterface
{
public:
  GripperInterface() : _open(true) {}

  NodeStatus open()
  {
    _open = true;
    std::cout << "GripperInterface::open" << std::endl;
    return NodeStatus::SUCCESS;
  }

  NodeStatus close()
  {
    std::cout << "GripperInterface::close" << std::endl;
    _open = false;
    return NodeStatus::SUCCESS;
  }

private:
  bool _open;  // 共享信息
};

}  // namespace DummyNodes
