// ============================================================================
// 教程 04：反应式行为（Sequence vs ReactiveSequence）
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_04_sequence
// ----------------------------------------------------------------------------
// 本节学习：
//   - Sequence 与 ReactiveSequence 的区别
//   - 如何写异步动作节点（StatefulActionNode，会返回 RUNNING）
//
// 关键区别：
//   - 用 Sequence 时，前面的子节点一旦返回 SUCCESS 就不会再被执行（只 tick 一次）；
//   - 用 ReactiveSequence 时，每次 tick 都会从头（最左边的子节点）重新执行，
//     因此条件节点会被反复检查。
// ============================================================================
#include "dummy_nodes.h"
#include "movebase_node.h"

#include "behaviortree_cpp/bt_factory.h"

using namespace BT;

// clang-format off
namespace
{
// 用普通 Sequence：BatteryOK 只会被执行一次
const char* xml_text_sequence = R"(
 <root BTCPP_format="4" >
     <BehaviorTree ID="MainTree">
        <Sequence name="root">
            <BatteryOK/>
            <SaySomething   message="mission started..." />
            <MoveBase       goal="1;2;3"/>
            <SaySomething   message="mission completed!" />
        </Sequence>
     </BehaviorTree>
 </root>
 )";

// 用 ReactiveSequence：每次 tick 都会重新检查 BatteryOK
const char* xml_text_reactive = R"(
 <root BTCPP_format="4" >
     <BehaviorTree ID="MainTree">
        <ReactiveSequence name="root">
            <BatteryOK/>
            <Sequence>
                <SaySomething   message="mission started..." />
                <MoveBase       goal="1;2;3"/>
                <SaySomething   message="mission completed!" />
            </Sequence>
        </ReactiveSequence>
     </BehaviorTree>
 </root>
 )";
}  // namespace
// clang-format on

using namespace DummyNodes;

int main()
{
  BehaviorTreeFactory factory;

  factory.registerSimpleCondition("BatteryOK", std::bind(CheckBattery));
  factory.registerNodeType<MoveBaseAction>("MoveBase");
  factory.registerNodeType<SaySomething>("SaySomething");

  // 依次运行上面两棵树，对比状态转移和输出即可看出区别。
  for(auto& xml_text : { xml_text_sequence, xml_text_reactive })
  {
    std::cout << "\n------------ BUILDING A NEW TREE ------------\n\n";

    auto tree = factory.createTreeFromText(xml_text);

    NodeStatus status = NodeStatus::IDLE;
    // 如果我们想在两次 tick 之间执行自己的代码，可以自己写循环：
    while(status != NodeStatus::SUCCESS)
    {
      std::cout << "--- ticking\n";
      status = tree.tickOnce();
      std::cout << "--- status: " << toStr(status) << "\n\n";

      // 还在 RUNNING 就稍微等一会儿
      if(status == NodeStatus::RUNNING)
      {
        tree.sleep(std::chrono::milliseconds(100));
      }
    }
  }
  return 0;
}
