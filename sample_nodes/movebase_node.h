// ============================================================================
// sample_nodes/movebase_node.h
// ----------------------------------------------------------------------------
// 教程 04 / 06 用到的“异步动作节点”示例：MoveBaseAction。
// 它继承 BT::StatefulActionNode，一次 tick 不会立即完成，
// 而是先返回 RUNNING，过一段时间后才返回 SUCCESS。
// 同时演示了如何为自定义类型 Pose2D 做字符串/JSON 转换。
// ============================================================================
#pragma once

#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/json_export.h"

// 自定义类型
struct Pose2D
{
  double x = 0;
  double y = 0;
  double theta = 0;
};

// 把这个放到 main() 里即可注册 JSON 转换：
//   BT::JsonExporter::get().addConverter<Pose2D>();
// NOLINTNEXTLINE(misc-use-internal-linkage)
BT_JSON_CONVERTER(Pose2D, pose)
{
  add_field("x", &pose.x);
  add_field("y", &pose.y);
  add_field("theta", &pose.theta);
}

namespace BT
{
// 只有希望在 XML/端口里把字符串自动转成 Pose2D 时，才需要这个特化。
template <>
inline Pose2D convertFromString(StringView key)
{
  // 三段实数，用分号隔开，例如 "1;2;3"
  auto parts = BT::splitString(key, ';');
  if(parts.size() != 3)
  {
    throw BT::RuntimeError("invalid input)");
  }
  else
  {
    Pose2D output;
    output.x = convertFromString<double>(parts[0]);
    output.y = convertFromString<double>(parts[1]);
    output.theta = convertFromString<double>(parts[2]);
    return output;
  }
}
}  // end namespace BT

namespace chr = std::chrono;

// 这是一个异步动作
class MoveBaseAction : public BT::StatefulActionNode
{
public:
  // 带端口的 TreeNode 必须有这个签名的构造函数
  MoveBaseAction(const std::string& name, const BT::NodeConfig& config)
    : StatefulActionNode(name, config)
  {}

  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<Pose2D>("goal") };
  }

  // 开始时调用一次
  BT::NodeStatus onStart() override;

  // 只要 onStart() 返回 RUNNING，就会反复调用，直到返回非 RUNNING
  BT::NodeStatus onRunning() override;

  // 被其它节点打断时的回调
  void onHalted() override;

private:
  Pose2D _goal;
  chr::system_clock::time_point _completion_time;
};
