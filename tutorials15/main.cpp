// ============================================================================
// 教程 15：Mock 与节点替换（Nodes Replacement）
// 官方文档：https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_15_replace_rules
// ----------------------------------------------------------------------------
// 本节学习：在做集成/单元测试时，把树里的某些真实节点替换成“替身”(mock)。
//   - 替换规则在创建树之前设置（创建树时生效）
//   - 可以按“节点名”或“通配符模式”匹配
//   - 两种 mock：
//       (1) 普通节点：注册一个同名 ID 的 dummy 节点
//       (2) 内置的可配置 BT::TestNode：指定返回状态、异步延时、后置脚本
//   - 规则可以手动加，也可以从 JSON 读进来
// ============================================================================
#include "dummy_nodes.h"

#include "behaviortree_cpp/bt_factory.h"

#include <chrono>
#include <iostream>

// clang-format off
namespace
{
const char* xml_text = R"(
<root BTCPP_format="4">

  <BehaviorTree ID="MainTree">
    <Sequence>
      <SaySomething name="talk" message="hello world"/>

      <SubTree ID="MySub" name="mysub"/>

      <Script name="set_message" code="msg:= 'the original message' "/>
      <SaySomething message="{msg}"/>

      <Sequence name="counting">
        <SaySomething message="1"/>
        <SaySomething message="2"/>
        <SaySomething message="3"/>
      </Sequence>
    </Sequence>
  </BehaviorTree>

  <BehaviorTree ID="MySub">
    <Sequence>
      <AlwaysSuccess name="action_subA"/>
      <AlwaysSuccess name="action_subB"/>
    </Sequence>
  </BehaviorTree>

</root>
 )";
}  // namespace
// clang-format on

int main()
{
  using namespace DummyNodes;
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<SaySomething>("SaySomething");
  factory.registerBehaviorTreeFromText(xml_text);

  // 先看“原树”应该是什么表现
  {
    auto tree = factory.createTree("MainTree");

    std::cout << "----- Nodes fullPath() -------\n";
    tree.applyVisitor(
        [](BT::TreeNode* node) { std::cout << node->fullPath() << std::endl; });

    std::cout << "\n------ Output (original) ------\n";
    tree.tickWhileRunning();
  }

  //-----------------------------------------------------------------
  // mock 方式 (1)：注册一些“假”节点
  factory.registerSimpleAction("DummyAction", [](BT::TreeNode& self) {
    std::cout << "DummyAction substituting node with fullPath(): " << self.fullPath()
              << std::endl;
    return BT::NodeStatus::SUCCESS;
  });

  factory.registerSimpleAction("DummySaySomething", [](BT::TreeNode& self) {
    auto msg = self.getInput<std::string>("message");
    std::cout << "DummySaySomething: " << msg.value() << std::endl;
    return BT::NodeStatus::SUCCESS;
  });

  //-----------------------------------------------------------------
  // mock 方式 (2)：可配置的 BT::TestNode
  BT::TestNodeConfig test_config;
  test_config.return_status = BT::NodeStatus::SUCCESS;
  // 转成异步节点，等待 2000ms
  test_config.async_delay = std::chrono::milliseconds(2000);
  // 完成后执行这段后置脚本
  test_config.post_script = "msg := 'message SUBSTITUTED' ";

  // 同步版本（async_delay 为 0）
  BT::TestNodeConfig counting_config;
  counting_config.return_status = BT::NodeStatus::SUCCESS;

  //-----------------------------------------------------------------
  // 用 JSON 文件来定义替换规则（等价于手写 addSubstitutionRule）
  bool const USE_JSON = true;

  if(!USE_JSON)  // 手动添加替换规则
  {
    // 把匹配通配符 "mysub/action_*" 的节点替换成 DummyAction
    factory.addSubstitutionRule("mysub/action_*", "DummyAction");
    // 把名为 "talk" 的节点替换成 DummySaySomething
    factory.addSubstitutionRule("talk", "DummySaySomething");
    // 把 "set_message" 替换成配置好的 TestNode
    factory.addSubstitutionRule("set_message", test_config);
    // 也可以替换整个分支，比如 Sequence "counting"
    factory.addSubstitutionRule("counting", counting_config);
  }
  else
  {
    const char* json_text = R"(
    {
      "TestNodeConfigs": {
        "NewMessage": {
          "async_delay": 2000,
          "return_status": "SUCCESS",
          "post_script": "msg ='message SUBSTITUTED'"
        },
        "NoCounting": {
          "return_status": "SUCCESS"
        }
      },

      "SubstitutionRules": {
        "mysub/action_*": "DummyAction",
        "talk": "DummySaySomething",
        "set_message": "NewMessage",
        "counting": "NoCounting"
      }
    })";

    factory.loadSubstitutionRuleFromJSON(json_text);
  }

  //-----------------------------------------------------------------
  // 重要：所有替换必须在创建树之前设置好。
  // 建树时，工厂会用替换规则实例化 TestNode，而不是原始节点。
  auto tree = factory.createTree("MainTree");
  std::cout << "\n------ Output (substituted) ------\n";
  tree.tickWhileRunning();

  return 0;
}

/* 期望输出：

----- Nodes fullPath() -------
Sequence::1
talk
mysub
mysub/Sequence::4
mysub/action_subA
mysub/action_subB
set_message
SaySomething::8
counting
SaySomething::10
SaySomething::11
SaySomething::12

------ Output (original) ------
Robot says: hello world
Robot says: the original message
Robot says: 1
Robot says: 2
Robot says: 3

------ Output (substituted) ------
DummySaySomething: hello world
DummyAction substituting node with fullPath(): mysub/action_subA
DummyAction substituting node with fullPath(): mysub/action_subB
Robot says: message SUBSTITUTED

*/
