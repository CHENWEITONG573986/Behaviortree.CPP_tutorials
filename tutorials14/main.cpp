// ============================================================================
// 教程 14：子树模型（Subtree Models）与自动重映射
// 官方文档：https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_14_subtree_model
// ----------------------------------------------------------------------------
// 本节学习：
//   - 在 <TreeNodesModel> 里用 <SubTree ID="..."> 声明一棵子树的端口模型
//   - 给子树端口指定“默认重映射”和“是否必填”
//   - 调用方不填的端口就用模型里的默认值
// ============================================================================
#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/loggers/bt_cout_logger.h"

using namespace BT;

// clang-format off
namespace
{
// 子树定义 + 它的端口模型
const char* xml_subtree = R"(
<root BTCPP_format="4">

  <TreeNodesModel>
    <SubTree ID="MySub">
      <!-- 有默认值 42 -->
      <input_port name="sub_in_value" default="42"/>
      <!-- 没有默认值：调用方必须提供 -->
      <input_port name="sub_in_name"/>
      <!-- 默认重映射到父树的 {out_result} -->
      <output_port name="sub_out_result" default="{out_result}"/>
      <!-- 没有默认值：调用方必须提供 -->
      <output_port name="sub_out_state"/>
    </SubTree>
  </TreeNodesModel>

  <BehaviorTree ID="MySub">
    <Sequence>
      <ScriptCondition code="sub_in_value==42 && sub_in_name=='john'" />
      <Script code="sub_out_result:=69; sub_out_state:='ACTIVE'" />
    </Sequence>
  </BehaviorTree>
</root>
 )";

// 调用方：只显式重映射了没有默认值的两个端口
const char* xml_maintree = R"(
<root BTCPP_format="4">

  <BehaviorTree ID="MainTree">
    <Sequence>
      <Script code="in_name:= 'john' "/>
      <SubTree ID="MySub" sub_in_name="{in_name}"
                          sub_out_state="{out_state}"/>
      <ScriptCondition code=" out_result==69 && out_state=='ACTIVE' " />
    </Sequence>
  </BehaviorTree>

</root>
 )";
}  // namespace
// clang-format on

int main()
{
  BehaviorTreeFactory factory;
  factory.registerBehaviorTreeFromText(xml_subtree);
  factory.registerBehaviorTreeFromText(xml_maintree);

  auto tree = factory.createTree("MainTree");
  StdCoutLogger logger(tree);
  tree.tickWhileRunning();

  // 期望整个 Sequence 成功。
  //
  // 最终的重映射是：
  //   sub_in_name    <-> {in_name}      // 调用方 MainTree 指定
  //   sub_in_value   <-> 42             // 模型里的默认值
  //   sub_out_result <-> {out_result}   // 模型里的默认重映射
  //   sub_out_state  <-> {out_state}    // 调用方 MainTree 指定
  return 0;
}
