# 教程 01：你的第一棵行为树

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_01_first_tree>

## 本节要点

- **ActionNode 的创建方式**：推荐用继承（`BT::SyncActionNode`）并重写 `tick()`；
  也可以把一个普通函数或类的成员函数“注入”成节点（`registerSimpleAction` /
  `registerSimpleCondition`）。
- 每个节点都有一个人可读的 `name`（不必唯一）。
- `tick()` 是真正干活的地方，必须返回 `NodeStatus`：`RUNNING` / `SUCCESS` / `FAILURE`。
- 先用 `BehaviorTreeFactory` 注册节点，再用 XML 描述树，最后用 `createTreeFromFile()`
  创建并 `tickWhileRunning()` 执行。

## 这棵树

```
Sequence(name=root_sequence)
├── CheckBattery   (条件)
├── OpenGripper    (动作)
├── ApproachObject (动作)
└── CloseGripper   (动作)
```

`Sequence` 从左到右依次 `tick` 子节点，只要有一个失败就失败；全部成功才成功。

## 编译与运行

```bash
cd tutorials01
cmake -S . -B build
cmake --build build
./build/bt_tutorial_01        # 必须在 tutorials01 目录下运行（XML 是相对路径）
```

> 本机 VCPKG_ROOT 一般就是 `/home/rpp/vcpkg`。

## 期望输出

```
[ Battery: OK ]
GripperInterface::open
ApproachObject: approach_object
GripperInterface::close
```
