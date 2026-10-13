# 教程 05：使用子树（SubTree）

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_05_subtrees>

## 本节要点

- **SubTree**：把一棵已经定义好的 `BehaviorTree` 当作另一个控制节点的子节点来用，
  是组织大规模行为、复用逻辑的关键手段。
- 一个 XML 里可以写多个 `<BehaviorTree ID="...">`，用 `createTree("ID")` 指定主树。
- 控制节点/装饰器回顾：
  - `Fallback`（Selector）：从左到右尝试，任一成功即成功。
  - `Inverter`：把子节点结果取反。
  - `RetryUntilSuccessful num_attempts="N"`：失败就重试，最多 N 次。

## 这棵树

```
MainTree: Sequence
├── Fallback
│   ├── Inverter(IsDoorClosed)
│   └── SubTree(DoorClosed)
└── PassThroughDoor

DoorClosed: Fallback
├── OpenDoor
├── RetryUntilSuccessful(5) -> PickLock
└── SmashDoor
```

## 编译与运行

```bash
cd tutorials05
cmake -S . -B build
cmake --build build
./build/bt_tutorial_05
```

程序会先打印整棵树的结构（`printTreeRecursively`），再执行它。
