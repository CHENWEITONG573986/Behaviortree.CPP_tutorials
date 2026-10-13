// ============================================================================
// sample_nodes/movebase_node.cpp
// ----------------------------------------------------------------------------
// MoveBaseAction 的实现：模拟一个耗时约 220ms 的“导航到目标点”动作。
// ============================================================================
#include "movebase_node.h"

#include "behaviortree_cpp/bt_factory.h"

// 插件（运行时加载）入口
// NOLINTNEXTLINE(misc-use-anonymous-namespace,misc-use-internal-linkage)
BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<MoveBaseAction>("MoveBase");
}

BT::NodeStatus MoveBaseAction::onStart()
{
  if(!getInput<Pose2D>("goal", _goal))
  {
    throw BT::RuntimeError("missing required input [goal]");
  }
  printf("[ MoveBase: SEND REQUEST ]. goal: x=%.1f y=%.1f theta=%.1f\n", _goal.x, _goal.y,
         _goal.theta);

  // 用时间戳模拟动作需要 220ms 才能完成
  _completion_time = chr::system_clock::now() + chr::milliseconds(220);

  return BT::NodeStatus::RUNNING;
}

BT::NodeStatus MoveBaseAction::onRunning()
{
  // 假装在轮询动作是否完成（注意不要在 tick 里长时间阻塞）
  std::this_thread::sleep_for(chr::milliseconds(10));

  if(chr::system_clock::now() >= _completion_time)
  {
    std::cout << "[ MoveBase: FINISHED ]" << std::endl;
    return BT::NodeStatus::SUCCESS;
  }
  return BT::NodeStatus::RUNNING;
}

void MoveBaseAction::onHalted()
{
  printf("[ MoveBase: ABORTED ]");
}
