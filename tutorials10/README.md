# 教程 10：日志与观察者

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_10_observer>

## 本节要点

- **`BT::TreeObserver`**：挂在树上，统计每个节点被 tick 的次数、返回 SUCCESS / FAILURE 的次数。
  常用于写单元测试、校验状态转移是否符合预期。
- 每个节点有两个唯一标识：
  - `node->UID()`：整数 ID
  - `node->fullPath()`：人类可读的路径（会带上子树前缀，如 `mysub/action_subB`）
- 用 `tree.applyVisitor(lambda)` 遍历树上所有节点。

## 编译与运行

```bash
cd tutorials10
cmake -S . -B build
cmake --build build
./build/bt_tutorial_10
```

## 输出说明

程序先打印树的递归结构，再打印每个节点的 `UID -> fullPath`，
最后打印统计表：

```
[last_action]   T/S/F:  1/1/0
[mysub/action_subB]  T/S/F:  2/2/0
...
```

其中 `T` = transitions（tick 次数），`S` = success，`F` = failure。
