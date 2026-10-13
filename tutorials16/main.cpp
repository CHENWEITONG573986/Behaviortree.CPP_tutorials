// ============================================================================
// 教程 16：全局黑板惯用法（global blackboard idiom）
// 官方文档：https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_16_global_blackboard
// ----------------------------------------------------------------------------
// 本节学习：
//   - 默认情况下每棵子树的黑板是相互隔离的，除非做重映射
//   - 从 4.6 起，可以有一个“全局黑板”，在任何子树里都能直接用
//   - 语法：在键前加前缀 "@"，如 {@value}
//       "@" 表示“到层级最顶层的黑板里去找这个条目”
// ============================================================================
#include "behaviortree_cpp/bt_factory.h"

using namespace BT;

// clang-format off
namespace
{
const char* xml_main = R"(
<root BTCPP_format="4">

  <BehaviorTree ID="MainTree">
    <Sequence>
      <PrintNumber name="main_print" val="{@value}" />
      <SubTree ID="MySub"/>
    </Sequence>
  </BehaviorTree>

  <BehaviorTree ID="MySub">
    <Sequence>
      <PrintNumber name="sub_print" val="{@value}" />
      <Script code="@value_sqr := @value * @value" />
    </Sequence>
  </BehaviorTree>
</root>
 )";
}  // namespace
// clang-format on

class PrintNumber : public BT::SyncActionNode
{
public:
  PrintNumber(const std::string& name, const BT::NodeConfig& config)
    : BT::SyncActionNode(name, config)
  {}

  NodeStatus tick() override
  {
    const int val = getInput<int>("val").value();
    // 如果你不想用端口、想直接访问顶层黑板，也可以用：
    //   val = config().blackboard->get<int>("@value");
    std::cout << "[" << name() << "] val: " << val << std::endl;
    return NodeStatus::SUCCESS;
  }

  static BT::PortsList providedPorts()
  {
    return { BT::InputPort<int>("val") };
  }
};

//---------------------------------------------------
int main()
{
  BehaviorTreeFactory factory;

  factory.registerNodeType<PrintNumber>("PrintNumber");
  factory.registerBehaviorTreeFromText(xml_main);

  // 这个黑板“不属于任何树”，它就是全局黑板
  auto global_blackboard = BT::Blackboard::create();
  // MainTree 的黑板以 global_blackboard 作为父级
  auto root_blackboard = BT::Blackboard::create(global_blackboard);

  auto tree = factory.createTree("MainTree", root_blackboard);

  // 我们可以直接操作全局黑板
  for(int i = 1; i <= 3; i++)
  {
    global_blackboard->set("value", i);
    tree.tickOnce();
    int value_sqr = global_blackboard->get<int>("value_sqr");
    std::cout << "[While loop] value: " << i << " value_sqr: " << value_sqr << "\n\n";
  }

  return 0;
}

/* 期望输出：

[main_print] val: 1
[sub_print] val: 1
[While loop] value: 1 value_sqr: 1

[main_print] val: 2
[sub_print] val: 2
[While loop] value: 2 value_sqr: 4

[main_print] val: 3
[sub_print] val: 3
[While loop] value: 3 value_sqr: 9

*/
