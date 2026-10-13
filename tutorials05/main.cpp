// ============================================================================
// 教程 05：使用子树（SubTree）
// 官方文档：https://www.behaviortree.dev/docs/tutorial-basics/tutorial_05_subtrees
// ----------------------------------------------------------------------------
// 本节学习：
//   - 用 Fallback（回退）/ Decorator（装饰器）/ SubTree（子树）组合复杂行为
//   - 一个 XML 里可以定义多棵树，通过 ID 引用
//   - SubTree 让行为可以复用（这里我们先不关注端口重映射）
//
// 场景：先判断门是否关着，关着就走 "DoorClosed" 子树把门弄开，然后穿过门。
// ============================================================================
#include "crossdoor_nodes.h"

#include "behaviortree_cpp/bt_factory.h"

// clang-format off
static const char* xml_text = R"(
<root BTCPP_format="4">

    <BehaviorTree ID="MainTree">
        <Sequence>
            <Fallback>
                <Inverter>
                    <IsDoorClosed/>
                </Inverter>
                <SubTree ID="DoorClosed"/>
            </Fallback>
            <PassThroughDoor/>
        </Sequence>
    </BehaviorTree>

    <!-- 这两个树也可以写在别的文件里，用 <include> 或分别 register 再引用 -->
    <BehaviorTree ID="DoorClosed">
        <Fallback>
            <OpenDoor/>
            <RetryUntilSuccessful num_attempts="5">
                <PickLock/>
            </RetryUntilSuccessful>
            <SmashDoor/>
        </Fallback>
    </BehaviorTree>

</root>
 )";
// clang-format on

int main()
{
  BT::BehaviorTreeFactory factory;

  CrossDoor cross_door;
  cross_door.registerNodes(factory);

  // 一个 XML 里含多棵 <BehaviorTree> 时，先注册 XML，
  // 再用树的 ID 指定要实例化哪一棵。
  factory.registerBehaviorTreeFromText(xml_text);
  auto tree = factory.createTree("MainTree");

  // 打印整棵树的结构（方便对照理解）
  BT::printTreeRecursively(tree.rootNode());

  // 一直 tick，直到返回 SUCCESS 或 FAILURE
  tree.tickWhileRunning();

  return 0;
}

/* 流程说明：
 * MainTree 的 Sequence：
 *   Fallback:
 *     - Inverter(IsDoorClosed)：门开着 -> IsDoorClosed 返回 FAILURE -> 取反成 SUCCESS，
 *       于是 Fallback 成功，跳过 DoorClosed 子树。
 *     - 门关着 -> 进入 DoorClosed 子树：
 *         OpenDoor（门锁着会失败）-> RetryUntilSuccessful(PickLock)（第 3 次撬开）
 *         -> SmashDoor（兜底，总能成功）
 *   最后 PassThroughDoor 成功穿过门。
 */
