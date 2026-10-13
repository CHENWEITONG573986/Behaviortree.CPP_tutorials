// ============================================================================
// sample_nodes/crossdoor_nodes.h
// ----------------------------------------------------------------------------
// 教程 05 / 11 用到的“开门”示例节点集合，演示 Fallback / Decorator / SubTree。
// 所有方法都用成员函数绑定成 SimpleAction/SimpleCondition。
// ============================================================================
#pragma once

#include "behaviortree_cpp/bt_factory.h"

class CrossDoor
{
public:
  void registerNodes(BT::BehaviorTreeFactory& factory);

  void reset();

  // 门关着时返回 SUCCESS
  BT::NodeStatus isDoorClosed();

  // 成功穿过门时返回 SUCCESS
  BT::NodeStatus passThroughDoor();

  // 撬锁：第 3 次尝试后成功
  BT::NodeStatus pickLock();

  // 门锁着则 FAILURE
  BT::NodeStatus openDoor();

  // 总是能把门砸开
  BT::NodeStatus smashDoor();

private:
  bool _door_open = false;
  bool _door_locked = true;
  int _pick_attempts = 0;
};
