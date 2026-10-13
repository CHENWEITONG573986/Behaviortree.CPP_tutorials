// ============================================================================
// 教程 06：子树端口重映射（Port Remapping）
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_06_subtree_ports
// ----------------------------------------------------------------------------
// 本节学习：
//   - 每棵树（含子树）都有自己的黑板，默认互相隔离，避免命名冲突
//   - 通过“重映射”把子树的“内部端口”和父树的“外部黑板条目”连起来
//   - 语法：<SubTree ID="..." 内部端口名="{父树黑板键}" />
// ============================================================================
#include "dummy_nodes.h"
#include "movebase_node.h"

#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/loggers/bt_cout_logger.h"

// clang-format off
static const char* xml_text = R"(
<root BTCPP_format="4">

    <BehaviorTree ID="MainTree">
        <Sequence>
            <Script code=" move_goal='1;2;3' " />
            <SubTree ID="MoveRobot" target="{move_goal}" result="{move_result}" />
            <SaySomething message="{move_result}"/>
        </Sequence>
    </BehaviorTree>

    <BehaviorTree ID="MoveRobot">
        <Fallback>
            <Sequence>
                <MoveBase  goal="{target}"/>
                <Script code=" result:='goal reached' " />
            </Sequence>
            <ForceFailure>
                <Script code=" result:='error' " />
            </ForceFailure>
        </Fallback>
    </BehaviorTree>

</root>
 )";
// clang-format on

using namespace BT;
using namespace DummyNodes;

int main()
{
  BehaviorTreeFactory factory;

  factory.registerNodeType<SaySomething>("SaySomething");
  factory.registerNodeType<MoveBaseAction>("MoveBase");

  factory.registerBehaviorTreeFromText(xml_text);
  auto tree = factory.createTree("MainTree");

  tree.tickWhileRunning();

  // 打印两个黑板里目前有哪些内容，直观看“重映射”的结果
  std::cout << "\n------ First BB ------" << std::endl;
  tree.subtrees[0]->blackboard->debugMessage();
  std::cout << "\n------ Second BB------" << std::endl;
  tree.subtrees[1]->blackboard->debugMessage();

  return 0;
}

/* 期望输出：

------ First BB ------
move_result (std::string)
move_goal (Pose2D)

------ Second BB------
[result] remapped to port of parent tree [move_result]
[target] remapped to port of parent tree [move_goal]

*/
