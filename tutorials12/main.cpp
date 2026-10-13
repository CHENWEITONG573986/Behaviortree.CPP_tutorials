// ============================================================================
// 教程 12：端口的默认值
// 官方文档：https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_12_default_ports
// ----------------------------------------------------------------------------
// 本节学习：端口“默认值”的所有写法（当 XML 里没有给该端口赋值时生效）。
//   1) 直接给一个值                  InputPort<T>("port", T{...}, "...")
//   2) 指向黑板里的某个键            InputPort<T>("port", "{key}", "...")
//   3) 给一个字符串，运行时再转换    InputPort<T>("port", "5,6", "...")
//   4) {=} ：默认值就是同名黑板键    InputPort<T>("port", "{=}", "...")
//   5) JSON 形式                    InputPort<T>(R"(json:{...})", "...")
// ============================================================================
#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/json_export.h"

#include <iostream>
#include <stdexcept>
#include <string>

// 自定义类型，让例子更有意思
struct Point2D
{
  int x = 0;
  int y = 0;
  bool operator==(const Point2D& p) const { return x == p.x && y == p.y; }
  bool operator!=(const Point2D& p) const { return !(*this == p); }
};

// 生成 Point2D <-> JSON 的双向转换代码
BT_JSON_CONVERTER(Point2D, point)
{
  add_field("x", &point.x);
  add_field("y", &point.y);
}

// 扩展 BT::convertFromString<Point2D>()，让它也支持 JSON 前缀（见 pointE）
template <>
[[nodiscard]] Point2D BT::convertFromString<Point2D>(StringView str)
{
  if(StartWith(str, "json:"))
  {
    str.remove_prefix(5);
    return convertFromJSON<Point2D>(str);
  }
  const auto parts = BT::splitString(str, ',');
  if(parts.size() != 2)
  {
    throw BT::RuntimeError("invalid input)");
  }
  int x = convertFromString<int>(parts[0]);
  int y = convertFromString<int>(parts[1]);
  return { x, y };
}

//-----------------------------------------------
using namespace BT;

class NodeWithDefaultPoints : public SyncActionNode
{
public:
  NodeWithDefaultPoints(const std::string& name, const NodeConfig& config)
    : SyncActionNode(name, config)
  {}

  NodeStatus tick() override
  {
    // 检查所有端口的默认值是否符合预期
    Point2D pointA, pointB, pointC, pointD, pointE, input;

    if(!getInput("pointA", pointA) || pointA != Point2D{ 1, 2 })
    {
      throw std::runtime_error("failed pointA");
    }
    if(!getInput("pointB", pointB) || pointB != Point2D{ 3, 4 })
    {
      throw std::runtime_error("failed pointB");
    }
    if(!getInput("pointC", pointC) || pointC != Point2D{ 5, 6 })
    {
      throw std::runtime_error("failed pointC");
    }
    if(!getInput("pointD", pointD) || pointD != Point2D{ 7, 8 })
    {
      throw std::runtime_error("failed pointD");
    }
    if(!getInput("pointE", pointE) || pointE != Point2D{ 9, 10 })
    {
      throw std::runtime_error("failed pointE");
    }
    if(!getInput("input", input) || input != Point2D{ -1, -2 })
    {
      throw std::runtime_error("failed input");
    }
    return NodeStatus::SUCCESS;
  }

  static PortsList providedPorts()
  {
    return { BT::InputPort<Point2D>("input", "no default value"),
             BT::InputPort<Point2D>("pointA", Point2D{ 1, 2 }, "default value is [1,2]"),
             BT::InputPort<Point2D>("pointB", "{point}",
                                    "default value inside blackboard {point}"),
             BT::InputPort<Point2D>("pointC", "5,6", "default value is [5,6]"),
             BT::InputPort<Point2D>("pointD", "{=}",
                                    "default value inside blackboard {pointD}"),
             BT::InputPort<Point2D>("pointE", R"(json:{"x":9,"y":10})",
                                    "default value is [9,10]") };
  }
};

int main()
{
  std::string xml_txt = R"(
    <root BTCPP_format="4" >
      <BehaviorTree>
        <NodeWithDefaultPoints input="-1,-2"/>
      </BehaviorTree>
    </root>)";

  JsonExporter::get().addConverter<Point2D>();

  BehaviorTreeFactory factory;
  factory.registerNodeType<NodeWithDefaultPoints>("NodeWithDefaultPoints");
  auto tree = factory.createTreeFromText(xml_txt);

  // pointB 的默认值是 {point}，pointD 的默认值是 {=}（即同名键 pointD），
  // 所以这里要先把这两个黑板条目准备好。
  tree.subtrees.front()->blackboard->set<Point2D>("point", Point2D{ 3, 4 });
  tree.subtrees.front()->blackboard->set<Point2D>("pointD", Point2D{ 7, 8 });

  BT::NodeStatus status = tree.tickOnce();
  std::cout << "Result: " << toStr(status) << std::endl;

  return 0;
}

/* 期望输出：
 *   Result: SUCCESS
 * 说明所有默认值都解析正确；若某个端口不符合预期，tick 里会抛异常。
 */
