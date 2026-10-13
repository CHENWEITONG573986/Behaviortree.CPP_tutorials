// ============================================================================
// sample_nodes/crossdoor_nodes.cpp
// ----------------------------------------------------------------------------
// CrossDoor 的实现。用 sleep 模拟每个动作的耗时。
// ============================================================================
#include "crossdoor_nodes.h"

#include <chrono>
#include <thread>

namespace
{
void SleepMS(int ms)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
}  // namespace

using BT::NodeStatus;

NodeStatus CrossDoor::isDoorClosed()
{
  if(_door_open)
  {
    std::cout << "门开着" << std::endl;
  }
  else
  {
    std::cout << "门关着" << std::endl;
  }
  SleepMS(200);
  return !_door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus CrossDoor::passThroughDoor()
{
  SleepMS(500);
  return _door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus CrossDoor::openDoor()
{
  SleepMS(500);
  if(_door_locked)
  {
    std::cout << "门打不开" << std::endl;
    return NodeStatus::FAILURE;
  }
  else
  {
    std::cout << "门打开了" << std::endl;
    _door_open = true;
    return NodeStatus::SUCCESS;
  }
}

NodeStatus CrossDoor::pickLock()
{
  SleepMS(500);
  std::cout << "第" << ++_pick_attempts << "次撬门" << std::endl;
  // 第 3 次尝试后成功
  if(_pick_attempts >= 3)
  {
    _door_locked = false;
    _door_open = true;
    std::cout << "门撬开了" << std::endl;
  }
  return _door_open ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}

NodeStatus CrossDoor::smashDoor()
{
  _door_locked = false;
  _door_open = true;
  std::cout << "把门砸开" << std::endl;
  // 砸门总能成功
  return NodeStatus::SUCCESS;
}

void CrossDoor::registerNodes(BT::BehaviorTreeFactory& factory)
{
  factory.registerSimpleCondition("IsDoorClosed",
                                  std::bind(&CrossDoor::isDoorClosed, this));

  factory.registerSimpleAction("PassThroughDoor",
                               std::bind(&CrossDoor::passThroughDoor, this));

  factory.registerSimpleAction("OpenDoor", std::bind(&CrossDoor::openDoor, this));

  factory.registerSimpleAction("PickLock", std::bind(&CrossDoor::pickLock, this));

  factory.registerSimpleCondition("SmashDoor", std::bind(&CrossDoor::smashDoor, this));
}

void CrossDoor::reset()
{
  _door_open = false;
  _door_locked = true;
  _pick_attempts = 0;
}

// 插件（运行时加载）入口
// NOLINTNEXTLINE(misc-use-anonymous-namespace,misc-use-internal-linkage)
BT_REGISTER_NODES(factory)
{
  static CrossDoor cross_door;
  cross_door.registerNodes(factory);
}
