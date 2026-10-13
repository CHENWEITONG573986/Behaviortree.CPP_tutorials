// ============================================================================
// 教程 03：带自定义类型的端口
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_03_generic_ports
// ----------------------------------------------------------------------------
// 本节学习：
//   - 端口类型不是 std::string 时怎么处理（这里用自定义结构体 Position2D）
//   - 为自定义类型特化 BT::convertFromString<>，让字符串能自动转成该类型
//   - 内置的 <Script> 动作也能给黑板写值（会自动做类型转换）
// ============================================================================
#include "behaviortree_cpp/bt_factory.h"

using namespace BT;

// 我们想在端口里使用的自定义类型
struct Position2D
{
  double x, y;
};

// 建议（有时是必须）为 convertFromString 提供一个模板特化，
// 把字符串转成 Position2D。
namespace BT
{
template <>
inline Position2D convertFromString(StringView str)
{
  printf("Converting string: \"%s\"\n", str.data());

  // 两个实数，用分号隔开
  auto parts = splitString(str, ';');
  if(parts.size() != 2)
  {
    throw RuntimeError("invalid input)");
  }
  else
  {
    Position2D output;
    output.x = convertFromString<double>(parts[0]);
    output.y = convertFromString<double>(parts[1]);
    return output;
  }
}
}  // end namespace BT

class CalculateGoal : public SyncActionNode
{
public:
  CalculateGoal(const std::string& name, const NodeConfig& config)
    : SyncActionNode(name, config)
  {}

  NodeStatus tick() override
  {
    Position2D mygoal = { 1.1, 2.3 };
    setOutput("goal", mygoal);
    return NodeStatus::SUCCESS;
  }

  static PortsList providedPorts()
  {
    return { OutputPort<Position2D>("goal") };
  }
};

class PrintTarget : public SyncActionNode
{
public:
  PrintTarget(const std::string& name, const NodeConfig& config)
    : SyncActionNode(name, config)
  {}

  NodeStatus tick() override
  {
    auto res = getInput<Position2D>("target");
    if(!res)
    {
      throw RuntimeError("error reading port [target]:", res.error());
    }
    Position2D goal = res.value();
    printf("Target positions: [ %.1f, %.1f ]\n", goal.x, goal.y);
    return NodeStatus::SUCCESS;
  }

  static PortsList providedPorts()
  {
    // 端口还可以带一段人类可读的描述
    const char* description = "Simply print the target on console...";
    return { InputPort<Position2D>("target", description) };
  }
};

//----------------------------------------------------------------
/** 这棵树是一个 4 个动作的 Sequence：
 *
 *  1) CalculateGoal 往黑板条目 "GoalPosition" 写入一个 Position2D
 *  2) PrintTarget 从黑板条目 "GoalPosition" 读取 target
 *  3) 内置动作 <Script> 往黑板写 "OtherGoal"，字符串会被自动转成 Position2D
 *  4) PrintTarget 从黑板条目 "OtherGoal" 读取 target
 */

// clang-format off
static const char* xml_text = R"(
 <root BTCPP_format="4" >
     <BehaviorTree ID="MainTree">
        <Sequence name="root">
            <CalculateGoal   goal="{GoalPosition}" />
            <PrintTarget     target="{GoalPosition}" />
            <Script          code="OtherGoal='-1;3'" />
            <PrintTarget     target="{OtherGoal}" />
        </Sequence>
     </BehaviorTree>
 </root>
 )";
// clang-format on

int main()
{
  using namespace BT;

  BehaviorTreeFactory factory;
  factory.registerNodeType<CalculateGoal>("CalculateGoal");
  factory.registerNodeType<PrintTarget>("PrintTarget");

  auto tree = factory.createTreeFromText(xml_text);

  tree.tickWhileRunning();

  /* 期望输出：
   *
   *   Target positions: [ 1.1, 2.3 ]
   *   Converting string: "-1;3"
   *   Target positions: [ -1.0, 3.0 ]
   */
  return 0;
}
