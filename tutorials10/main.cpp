// ============================================================================
// 教程 10：日志与观察者（Loggers & Observer）
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_10_observer
// ----------------------------------------------------------------------------
// 本节学习：
//   - 用 BT::TreeObserver 统计每个节点被执行了多少次、成功/失败多少次
//   - 用 node->UID() 和 node->fullPath() 唯一定位一个节点
//   - 这类统计信息在写单元测试、验证状态转移时非常有用
// ============================================================================
#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/loggers/bt_observer.h"

#include <cassert>
#include <iostream>
#include <map>
#include <string>

// clang-format off
static const char* xml_text = R"(
<root BTCPP_format="4">

    <BehaviorTree ID="MainTree">
        <Sequence>
            <Fallback>
                <AlwaysFailure name="failing_action"/>
                <SubTree ID="SubTreeA" name="mysub"/>
            </Fallback>
            <AlwaysSuccess name="last_action"/>
        </Sequence>
    </BehaviorTree>

    <BehaviorTree ID="SubTreeA">
        <Sequence>
            <AlwaysSuccess name="action_subA"/>
            <SubTree ID="SubTreeB" name="sub_nested"/>
            <SubTree ID="SubTreeB" />
        </Sequence>
    </BehaviorTree>

    <BehaviorTree ID="SubTreeB">
        <AlwaysSuccess name="action_subB"/>
    </BehaviorTree>

</root>
 )";
// clang-format on

int main()
{
  BT::BehaviorTreeFactory factory;

  factory.registerBehaviorTreeFromText(xml_text);
  auto tree = factory.createTree("MainTree");

  // 打印整棵树的结构
  BT::printTreeRecursively(tree.rootNode());

  // Observer 用来保存某个节点返回 SUCCESS/FAILURE 的统计次数
  BT::TreeObserver observer(tree);

  std::map<int, std::string> UID_to_path;

  // 打印每个节点的唯一 ID(UID) 和人类可读路径(fullPath)，两者都是唯一的
  tree.applyVisitor([&UID_to_path](BT::TreeNode* node) {
    UID_to_path[node->UID()] = node->fullPath();
    std::cout << node->UID() << " -> " << node->fullPath() << std::endl;
  });

  tree.tickWhileRunning();

  // 可以用 fullPath 或 UID 取某个节点的统计
  const auto& last_action_stats = observer.getStatistics("last_action");
  assert(last_action_stats.transitions_count > 0);

  std::cout << "----------------" << std::endl;
  // 打印所有统计： 执行次数 / 成功次数 / 失败次数
  for(const auto& [uid, name] : UID_to_path)
  {
    const auto& stats = observer.getStatistics(uid);

    std::cout << "[" << name << "] \tT/S/F:  " << stats.transitions_count << "/"
              << stats.success_count << "/" << stats.failure_count << std::endl;
  }

  return 0;
}
