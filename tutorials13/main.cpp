// ============================================================================
// 教程 13：按引用访问端口内容
// 官方文档：https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_13_blackboard_reference
// ----------------------------------------------------------------------------
// 本节学习：
//   - 用 getLockedPortContent() 直接拿到黑板条目（的指针），而不是拷贝一份值
//   - 这种访问是“加锁”的，在返回的指针有效期间对黑板的读写是互斥、线程安全的
//   - 适用于: 修改容器类数据（如 std::vector），避免每次拷贝
// ============================================================================
#include "behaviortree_cpp/bt_factory.h"

using namespace BT;

class PushIntoVector : public SyncActionNode
{
public:
  PushIntoVector(const std::string& name, const NodeConfig& config)
    : SyncActionNode(name, config)
  {}

  NodeStatus tick() override
  {
    const int number = getInput<int>("value").value();

    if(auto any_ptr = getLockedPortContent("vector"))
    {
      // 只要 any_ptr 还活着，对黑板条目的访问就是加锁、线程安全的。

      // 检查这个条目是否已经有有效内容
      if(any_ptr->empty())
      {
        // 还没有被其它节点初始化过，那就自己建一个 vector
        std::vector<int> vect = { number };
        any_ptr.assign(vect);
        std::cout << "We created a new vector, containing value [" << number << "]\n";
      }
      else if(auto* vect_ptr = any_ptr->castPtr<std::vector<int>>())
      {
        // 注意：如果类型转换失败，vect_ptr 会是 nullptr
        vect_ptr->push_back(number);
        std::cout << "Value [" << number
                  << "] pushed into the vector. New size: " << vect_ptr->size() << "\n";
      }
      return NodeStatus::SUCCESS;
    }
    else
    {
      return NodeStatus::FAILURE;
    }
  }

  static PortsList providedPorts()
  {
    return { BT::BidirectionalPort<std::vector<int>>("vector"),
             BT::InputPort<int>("value") };
  }
};

//--------------------------------------------------------------
// clang-format off
static const char* xml_tree = R"(
  <root BTCPP_format="4" >
    <BehaviorTree ID="TreeA">
      <Sequence>
        <PushIntoVector vector="{vect}" value="3"/>
        <PushIntoVector vector="{vect}" value="5"/>
        <PushIntoVector vector="{vect}" value="7"/>
      </Sequence>
    </BehaviorTree>
 </root>
 )";
// clang-format on

int main()
{
  BehaviorTreeFactory factory;
  factory.registerNodeType<PushIntoVector>("PushIntoVector");

  auto tree = factory.createTreeFromText(xml_tree);
  tree.tickWhileRunning();
  return 0;
}

/* 期望输出：
 *   We created a new vector, containing value [3]
 *   Value [5] pushed into the vector. New size: 2
 *   Value [7] pushed into the vector. New size: 3
 */
