// ============================================================================
// 教程 11：连接到 Groot2（图形化可视化 / 交互）
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_11_groot2
// ----------------------------------------------------------------------------
// 本节学习：
//   - 用 BT::writeTreeNodesModelXML / BT::WriteTreeToXML 导出节点模型与树结构
//   - 用 BT::Groot2Publisher 打开一个 TCP 端口，让 Groot2 IDE 连接并实时监视
//   - 用 FileLogger2 / MinitraceLogger 把状态转移记录到文件
//
// !!! 重要前提 !!!
//   Groot2Publisher 只有在 behaviortree_cpp 的 Groot2 接口被启用时才可用，
//   也就是构建时必须打开 BTCPP_GROOT_INTERFACE（vcpkg 里对应 feature "groot"）。
//   当前环境这个接口是关闭的，所以本目录的 CMake 会跳过可执行文件的编译。
//   如何启用见本节 README.md。
// ============================================================================
#include "crossdoor_nodes.h"

#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/json_export.h"
#include "behaviortree_cpp/loggers/bt_file_logger_v2.h"
#include "behaviortree_cpp/loggers/bt_minitrace_logger.h"
#include "behaviortree_cpp/loggers/groot2_publisher.h"
#include "behaviortree_cpp/xml_parsing.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// 想在 Groot2 里可视化自定义结构体时，用它生成 to/from JSON 的代码
struct Position2D
{
  double x;
  double y;
};

// NOLINTNEXTLINE(misc-use-internal-linkage)
BT_JSON_CONVERTER(Position2D, pos)
{
  add_field("x", &pos.x);
  add_field("y", &pos.y);
}

struct Waypoint
{
  std::string name;
  Position2D position;
  double speed = 1.0;
};

// NOLINTNEXTLINE(misc-use-internal-linkage)
BT_JSON_CONVERTER(Waypoint, wp)
{
  add_field("name", &wp.name);
  add_field("position", &wp.position);
  add_field("speed", &wp.speed);
}

// 一个简单的动作：更新黑板里的 Position2D，并输出一些向量
class UpdatePosition : public BT::SyncActionNode
{
public:
  UpdatePosition(const std::string& name, const BT::NodeConfig& config)
    : BT::SyncActionNode(name, config)
  {}

  BT::NodeStatus tick() override
  {
    _pos.x += 0.2;
    _pos.y += 0.1;
    setOutput("pos", _pos);

    std::vector<double> doubles = { 1.1, 2.2, 3.3, 4.4, 5.5 };
    setOutput("vec_double", doubles);

    std::vector<std::string> strings = { "hello", "world", "test" };
    setOutput("vec_string", strings);

    std::vector<Waypoint> waypoints;
    waypoints = { { "start", { 0.0, 0.0 }, 1.0 }, { "goal", { 10.0, 5.0 }, 0.5 } };
    setOutput("waypoints", waypoints);

    return BT::NodeStatus::SUCCESS;
  }

  static BT::PortsList providedPorts()
  {
    return {
      BT::OutputPort<Position2D>("pos"),
      BT::OutputPort<std::vector<double>>("vec_double", "Vector of doubles"),
      BT::OutputPort<std::vector<std::string>>("vec_string", "Vector of strings"),
      BT::OutputPort<std::vector<Waypoint>>("waypoints", "Vector of waypoints"),
    };
  }

private:
  Position2D _pos = { 0, 0 };
};

// clang-format off
namespace
{
const char* xml_text = R"(
<root BTCPP_format="4">

  <BehaviorTree ID="MainTree">
    <Sequence>
      <Script code="door_open:=false" />
      <UpdatePosition pos="{pos_2D}" vec_double="{doubles}" vec_string="{strings}" waypoints="{waypoints}"/>
      <Fallback>
        <Inverter>
          <IsDoorClosed/>
        </Inverter>
        <SubTree ID="DoorClosed" _autoremap="true" door_open="{door_open}"/>
      </Fallback>
      <PassThroughDoor/>
    </Sequence>
  </BehaviorTree>

  <BehaviorTree ID="DoorClosed">
    <Fallback name="tryOpen" _onSuccess="door_open:=true">
      <OpenDoor/>
        <RetryUntilSuccessful num_attempts="5">
          <PickLock/>
        </RetryUntilSuccessful>
      <SmashDoor/>
    </Fallback>
  </BehaviorTree>

</root>
 )";
}  // namespace
// clang-format on

int main()
{
  BT::BehaviorTreeFactory factory;

  CrossDoor cross_door;
  cross_door.registerNodes(factory);
  factory.registerNodeType<UpdatePosition>("UpdatePosition");

  // Groot2 需要知道你的节点模型，可用下面这条命令自动生成，不必手写：
  [[maybe_unused]] const std::string xml_models = BT::writeTreeNodesModelXML(factory);

  factory.registerBehaviorTreeFromText(xml_text);

  // 让 Groot2 能显示你的自定义类型
  BT::RegisterJsonDefinition<Position2D>();
  BT::RegisterJsonDefinition<Waypoint>();

  auto tree = factory.createTree("MainTree");

  std::cout << "----------- XML file  ----------\n"
            << BT::WriteTreeToXML(tree, false, false)
            << "--------------------------------\n";

  // 打开 Groot2 的通信端口：Groot2 连接它就能拿到树结构并轮询状态。
  const unsigned port = 1667;
  BT::Groot2Publisher publisher(tree, port);

  // 另外再加两个 logger，把状态转移写进文件（两种格式 Groot2 都能读）
  BT::FileLogger2 logger2(tree, "t11_groot_howto.btlog");
  BT::MinitraceLogger minilog(tree, "minitrace.json");

  // 官方示例这里是 while(true) 一直跑，等待 Groot2 连接。
  // 为了这个教程能自动结束，这里只循环 3 次。
  const int max_iterations = 3;
  for(int i = 0; i < max_iterations; i++)
  {
    std::cout << "Start" << std::endl;
    cross_door.reset();
    tree.tickWhileRunning();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  return 0;
}
