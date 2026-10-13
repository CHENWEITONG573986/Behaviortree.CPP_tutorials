# 教程 09：脚本（Scripting）

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_09_scripting>

## 本节要点

- 内置节点 **`<Script code="...">`** 让你直接在 XML 里写赋值/算术表达式，例如
  `msg:='hello world'`、`A:=THE_ANSWER`、`color:=RED`。
- 脚本里所有数值都是 `double`。
- **`<Precondition if="..." else="FAILURE">`**：条件为真才 tick 子节点，
  否则返回 `else` 指定的状态。
- 用 `factory.registerScriptingEnums<Color>()` 把 C++ 枚举注册进脚本；
  或用 `registerScriptingEnum("NAME", value)` 注册单个常量。

## 编译与运行

```bash
cd tutorials09
cmake -S . -B build
cmake --build build
./build/bt_tutorial_09
```

## 期望输出

```
Robot says: 42.000000
Robot says: 3.140000
Robot says: hello world
Robot says: 1.000000
```
