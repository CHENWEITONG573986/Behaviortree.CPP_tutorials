// ============================================================================
// 教程 01：你的第一棵行为树
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_01_first_tree
// ----------------------------------------------------------------------------
// 本节学习：
//   1. 用继承 TreeNode 的方式自定义同步动作节点
//   2. 用普通函数 / 类的成员函数包装成节点（依赖注入）
//   3. 把节点注册到工厂，再用 XML 描述一棵树
//   4. 通过 tick 执行这棵树
// ============================================================================
#include "behaviortree_cpp/bt_factory.h"

// 自定义节点都放在这个头文件里
#include "dummy_nodes.h"
using namespace DummyNodes;

int main()
{
  // BehaviorTreeFactory 负责保存“节点名 -> 节点实现”的映射
  BT::BehaviorTreeFactory factory;

  // 方式一（推荐）：通过继承自定义节点
  factory.registerNodeType<ApproachObject>("ApproachObject");

  // 方式二：用一个普通函数包装成“条件节点”（SimpleConditionNode）
  factory.registerSimpleCondition("CheckBattery", [&](BT::TreeNode&) { return CheckBattery(); });

  // 方式三：用类的成员函数包装成“动作节点”（SimpleActionNode）
  GripperInterface gripper;
  factory.registerSimpleAction("OpenGripper", [&](BT::TreeNode&) { return gripper.open(); });
  factory.registerSimpleAction("CloseGripper", [&](BT::TreeNode&) { return gripper.close(); });

  // 树在“部署期”（运行时，但只在开头一次）根据 XML 创建。
  // 注意：tree 析构时，所有 TreeNode 都会被销毁。
  // XML 用的是相对路径，所以请在 tutorials01 目录下运行程序。
  auto tree = factory.createTreeFromFile("./my_tree.xml");

  // 执行一棵树靠的是反复 tick：信号会按树的逻辑向下传播。
  // 这里整个 Sequence 的每个子节点都返回 SUCCESS，所以会被完整执行一遍。
  tree.tickWhileRunning();

  return 0;
}

/* 期望输出：
 *
 *  [ Battery: OK ]
 *  GripperInterface::open
 *  ApproachObject: approach_object
 *  GripperInterface::close
 */
