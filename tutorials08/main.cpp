// ============================================================================
// 教程 08：给节点传额外参数
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_08_additional_args
// ----------------------------------------------------------------------------
// 本节学习：
//   - 当某些参数在“编译期/部署期”就确定、运行时不会变时，用输入端口反而累赘，
//     可以直接把它们作为构造函数的额外参数传进去。
//   - 两种做法：
//       (A) 自定义构造函数并接收额外参数，注册时一并传入
//       (B) 先正常构造，再通过 initialize() 之类的方法在第一次 tick 之前初始化
// ============================================================================
#include "behaviortree_cpp/bt_factory.h"

#include <functional>
#include <iostream>
#include <string>

using namespace BT;

// 为了演示“按引用传参”，用一个不可拷贝的对象
class NoCopyObj
{
public:
  NoCopyObj(int val) : _value(val) {}

  NoCopyObj(const NoCopyObj&) = delete;
  NoCopyObj& operator=(const NoCopyObj&) = delete;

  int value() { return _value; }

private:
  int _value = 0;
};

// 做法 A：Action_A 的构造函数比默认的多几个参数
class Action_A : public SyncActionNode
{
public:
  Action_A(const std::string& name, const NodeConfig& config, int arg_int,
           std::string arg_str, NoCopyObj& nc)
    : SyncActionNode(name, config), _arg1(arg_int), _arg2(arg_str), _nc(nc)
  {}

  NodeStatus tick() override
  {
    std::cout << name() << ": " << _arg1 << " / " << _arg2 << " / " << _nc.value()
              << std::endl;
    return NodeStatus::SUCCESS;
  }

  static PortsList providedPorts() { return {}; }

private:
  int _arg1;
  std::string _arg2;
  NoCopyObj& _nc;
};

// 做法 B：Action_B 提供一个 init(...) 方法，必须在第一次 tick() 之前调用一次
class Action_B : public SyncActionNode
{
public:
  Action_B(const std::string& name, const NodeConfig& config)
    : SyncActionNode(name, config)
  {}

  void initialize(int arg_int, std::string arg_str)
  {
    _arg1 = arg_int;
    _arg2 = arg_str;
  }

  NodeStatus tick() override
  {
    std::cout << name() << ": " << _arg1 << " / " << _arg2 << std::endl;
    return NodeStatus::SUCCESS;
  }

  static PortsList providedPorts() { return {}; }

private:
  int _arg1 = 0;
  std::string _arg2;
};

static const char* xml_text = R"(
 <root BTCPP_format="4">
     <BehaviorTree>
        <Sequence>
            <Action_A/>
            <Action_B/>
        </Sequence>
     </BehaviorTree>
 </root>
 )";

int main()
{
  BehaviorTreeFactory factory;

  NoCopyObj non_copyable(88);

  // 把额外参数传给 Action_A 的构造函数。
  // 注意：如果想按引用传对象（而不是拷贝），要用 std::ref 包一层。
  factory.registerNodeType<Action_A>("Action_A", 42, "hello world",
                                     std::ref(non_copyable));

  // Action_B 之后再初始化
  factory.registerNodeType<Action_B>("Action_B");

  auto tree = factory.createTreeFromText(xml_text);

  // 用 applyVisitor 遍历整棵树，找到 Action_B 手动调用 initialize()
  auto visitor = [](TreeNode* node) {
    if(auto action_B_node = dynamic_cast<Action_B*>(node))
    {
      action_B_node->initialize(69, "interesting_value");
    }
  };
  tree.applyVisitor(visitor);

  tree.tickWhileRunning();

  /* 期望输出：
   *   Action_A: 42 / hello world / 88
   *   Action_B: 69 / interesting_value
   */
  return 0;
}
