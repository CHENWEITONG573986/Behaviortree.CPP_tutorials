// ============================================================================
// 教程 09：脚本（Scripting）
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_09_scripting
// ----------------------------------------------------------------------------
// 本节学习：
//   - 用内置的 <Script> 在 XML 里直接给黑板赋值（支持算术、比较、赋值等）
//   - 用 <Precondition> 做条件判断（不满足时 else 分支，这里是 FAILURE）
//   - 通过 registerScriptingEnum / registerScriptingEnums 把 C++ 枚举引入脚本
// ============================================================================
#include "dummy_nodes.h"

#include "behaviortree_cpp/bt_factory.h"

using namespace BT;

// clang-format off
namespace
{
const char* xml_text = R"(
 <root BTCPP_format="4">
     <BehaviorTree>
        <Sequence>
            <Script code=" msg:='hello world' " />
            <Script code=" A:=THE_ANSWER; B:=3.14; color:=RED " />
            <Precondition if="A>-B && color != BLUE" else="FAILURE">
                <Sequence>
                  <SaySomething message="{A}"/>
                  <SaySomething message="{B}"/>
                  <SaySomething message="{msg}"/>
                  <SaySomething message="{color}"/>
                </Sequence>
            </Precondition>
        </Sequence>
     </BehaviorTree>
 </root>
 )";
}  // namespace
// clang-format on

int main()
{
  BehaviorTreeFactory factory;
  factory.registerNodeType<DummyNodes::SaySomething>("SaySomething");

  enum Color
  {
    RED = 1,
    BLUE = 2,
    GREEN = 3
  };
  // 把枚举 Color 引入脚本语言（RED/BLUE/GREEN）
  factory.registerScriptingEnums<Color>();

  // 也可以手动注册一个常量
  factory.registerScriptingEnum("THE_ANSWER", 42);

  auto tree = factory.createTreeFromText(xml_text);
  tree.tickWhileRunning();

  return 0;
}

/* 期望输出：
 *
 * Robot says: 42.000000
 * Robot says: 3.140000
 * Robot says: hello world
 * Robot says: 1.000000
 *
 * 注意：脚本里的数值按 double 处理，所以打印带小数。
 * color 被打印成 1（即 RED）。
 */
