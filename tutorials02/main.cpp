// ============================================================================
// 教程 02：黑板与端口
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_02_basic_ports
// ----------------------------------------------------------------------------
// 本节学习：
//   - 输入端口(Input Port) / 输出端口(Output Port) 的基本用法
//   - 端口之间是通过“黑板(Blackboard)”这个 key/value 存储来交换信息的
//   - 端口类型和数量在编译期就静态定义了
//       * 输入端口  ≈ 函数参数
//       * 输出端口  ≈ 返回值
//   - XML 里输入端口既可以写普通字符串，也可以写 {黑板键} 指向黑板条目
// ============================================================================
#include "dummy_nodes.h"

#include "behaviortree_cpp/bt_factory.h"

using namespace BT;

// 本树的执行顺序（一个 Sequence，5 个动作里其实只有 4 个节点）：
//   - 第 1 个动作从静态字符串读入 "message"
//   - 第 2 个动作也读静态字符串
//   - 第 3 个动作往黑板写 "the_answer"
//   - 第 4 个动作再从黑板读 "the_answer"
// clang-format off
static const char* xml_text = R"(
 <root BTCPP_format="4" >
     <BehaviorTree ID="MainTree">
        <Sequence name="root">
            <SaySomething     message="hello" />
            <SaySomething2    message="this works too" />
            <ThinkWhatToSay   text="{the_answer}"/>
            <SaySomething2    message="{the_answer}" />
        </Sequence>
     </BehaviorTree>
 </root>
 )";
// clang-format on

class ThinkWhatToSay : public BT::SyncActionNode
{
public:
  ThinkWhatToSay(const std::string& name, const BT::NodeConfig& config)
    : BT::SyncActionNode(name, config)
  {}

  // 这个动作只是往输出端口 "text" 写一个值
  BT::NodeStatus tick() override
  {
    setOutput("text", "The answer is 42");
    return BT::NodeStatus::SUCCESS;
  }

  // 有端口的节点必须实现这个静态方法
  static BT::PortsList providedPorts()
  {
    return { BT::OutputPort<std::string>("text") };
  }
};

int main()
{
  using namespace DummyNodes;

  BehaviorTreeFactory factory;

  // SaySomething 通过 providedPorts() 声明了输入端口 "message"
  factory.registerNodeType<SaySomething>("SaySomething");

  // ThinkWhatToSay 有一个输出端口 "text"
  factory.registerNodeType<ThinkWhatToSay>("ThinkWhatToSay");

  // SimpleActionNode 不能自己声明端口，所以要显式把 PortsList 传进去
  PortsList say_something_ports = { InputPort<std::string>("message") };
  factory.registerSimpleAction("SaySomething2", SaySomethingSimple, say_something_ports);

  /* 输入端口有两种写法：
   *   <SaySomething message="hello" />          —— 普通字符串
   *   <SaySomething message="{the_answer}" />   —— 指向黑板条目 the_answer
   */

  auto tree = factory.createTreeFromText(xml_text);

  tree.tickWhileRunning();

  /* 期望输出（注意第 2、3 个动作的输出）：
   *
   *   Robot says: hello
   *   Robot says: this works too
   *   Robot says: The answer is 42
   *
   * 端口之间通过“指向同一个黑板条目”来连接：
   * ThinkWhatToSay 写入 the_answer，后面两个 SaySomething 从同一处读取。
   */
  return 0;
}
