# 教程 03：带自定义类型的端口

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_03_generic_ports>

## 本节要点

- 端口类型不限于 `std::string`，可以是任意类型（这里是自定义的 `Position2D`）。
- 想让字符串自动转换成自定义类型，需要为 `BT::convertFromString<Position2D>`
  提供模板特化（在 `namespace BT` 里）。
- 端口可以带描述文字（第三个参数），纯属给人和 Groot2 看的。
- 内置的 `<Script>` 动作也能给黑板写值，写入时同样走 `convertFromString`。
- 读端口用 `getInput<T>()`（返回 `Expected<T>`，失败要处理）；写端口用 `setOutput()`。

## 这棵树

```
Sequence(root)
├── CalculateGoal  goal="{GoalPosition}"    # 写 Position2D 到黑板
├── PrintTarget    target="{GoalPosition}"  # 读并打印
├── Script         code="OtherGoal='-1;3'"  # 字符串 "-1;3" 自动转成 Position2D
└── PrintTarget    target="{OtherGoal}"
```

## 编译与运行

```bash
cd tutorials03
cmake -S . -B build
cmake --build build
./build/bt_tutorial_03
```

## 期望输出

```
Target positions: [ 1.1, 2.3 ]
Converting string: "-1;3"
Target positions: [ -1.0, 3.0 ]
```
