// ============================================================================
// 教程 07：使用多个 XML 文件
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_07_multiple_xml
// ----------------------------------------------------------------------------
// 本节学习：
//   - 当子树越来越多时，把每棵（子）树拆到独立的 XML 文件里
//   - 手动把一个目录下所有 .xml 都注册进工厂（推荐做法）
//   - 也可以只在主文件里用 <include path="..."/>，让工厂自己去加载依赖
//
// 本目录里有三个文件：main_tree.xml / subtree_A.xml / subtree_B.xml
// ============================================================================
#include "dummy_nodes.h"

#include "behaviortree_cpp/bt_factory.h"

#include <filesystem>
#include <iostream>
#include <string>

using namespace BT;

int main()
{
  BT::BehaviorTreeFactory factory;
  factory.registerNodeType<DummyNodes::SaySomething>("SaySomething");

  // 把当前目录下所有 .xml 文件都注册进工厂（推荐做法）。
  // 这里用 std::filesystem::directory_iterator 遍历目录。
  const std::string search_directory = "./";

  for(const auto& entry : std::filesystem::directory_iterator(search_directory))
  {
    if(entry.path().extension() == ".xml")
    {
      factory.registerBehaviorTreeFromFile(entry.path().string());
    }
  }
  // 上面这段等价于手动写：
  //   factory.registerBehaviorTreeFromFile("./main_tree.xml");
  //   factory.registerBehaviorTreeFromFile("./subtree_A.xml");
  //   factory.registerBehaviorTreeFromFile("./subtree_B.xml");

  std::cout << "Registered BehaviorTrees:" << std::endl;
  for(const std::string& bt_name : factory.registeredBehaviorTrees())
  {
    std::cout << " - " << bt_name << std::endl;
  }

  // 创建 MainTree，它引用的子树会被自动补上
  std::cout << "----- MainTree tick ----" << std::endl;
  auto main_tree = factory.createTree("MainTree");
  main_tree.tickWhileRunning();

  // 也可以只创建其中一棵子树
  std::cout << "----- SubTreeA tick ----" << std::endl;
  auto subA_tree = factory.createTree("SubTreeA");
  subA_tree.tickWhileRunning();

  return 0;
}

/* 期望输出：

Registered BehaviorTrees:
 - MainTree
 - SubTreeA
 - SubTreeB
----- MainTree tick ----
Robot says: starting MainTree
Robot says: Executing Sub_A
Robot says: Executing Sub_B
----- SubTreeA tick ----
Robot says: Executing Sub_A

*/
