# 教程 04：反应式行为

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_04_sequence>

## 本节要点

- **Sequence**：从左到右执行，子节点返回 `SUCCESS` 后就“记住”它已完成，下次不再 tick 它。
- **ReactiveSequence**：每次 tick 都从最左边重新开始，前面的条件会被反复检查。
  这对于“随时监控某个条件”的场景非常重要（例如电池没电就中断任务）。
- **异步动作**：继承 `BT::StatefulActionNode`，实现 `onStart()` / `onRunning()` /
  `onHalted()`。第一次返回 `RUNNING`，之后在 `onRunning()` 里轮询直到完成。
  本节的 `MoveBase` 就是这样一个异步动作。

## 程序会依次运行两棵树

1. `xml_text_sequence`：普通 `Sequence`，`BatteryOK` 只检查一次。
2. `xml_text_reactive`：`ReactiveSequence`，每次 tick 都重新检查 `BatteryOK`。

通过打印的 `--- ticking` / `--- status` 就能看出两者被 tick 的节奏不同。

## 编译与运行

```bash
cd tutorials04
cmake -S . -B build
cmake --build build
./build/bt_tutorial_04
```

## 输出节选

```
------------ BUILDING A NEW TREE ------------
--- ticking
[ Battery: OK ]
Robot says: mission started...
[ MoveBase: SEND REQUEST ]. goal: x=1.0 y=2.0 theta=3.0
--- status: RUNNING
...
[ MoveBase: FINISHED ]
Robot says: mission completed!
--- status: SUCCESS
```

（第一棵树里 `[ Battery: OK ]` 只出现一次；第二棵树里每次 tick 都会出现。）
