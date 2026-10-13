# 教程 15：Mock 与节点替换

官方文档：<https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_15_replace_rules>

## 本节要点

做集成/单元测试时，可以把树里的真实节点替换成“替身”。要点：

- 替换规则必须在 **`createTree()` 之前**设置，建树时生效。
- 匹配方式：直接按 `name`，或按通配符，例如 `"mysub/action_*"`。
- 两类 mock：
  1. **普通替换**：注册一个用于替换的节点 ID（如 `DummyAction`）。
  2. **内置 `BT::TestNode`**：可配置
     - `return_status`：固定返回的状态
     - `async_delay`：延时（变成异步节点）
     - `post_script`：完成后执行的脚本
- 规则可以手动加 `factory.addSubstitutionRule(...)`，
  也可以用 `factory.loadSubstitutionRuleFromJSON(json_text)` 从 JSON 读。

## 编译与运行

```bash
cd tutorials15
cmake -S . -B build
cmake --build build
./build/bt_tutorial_15
```

程序会先跑“原树”，再跑“替换后”的树，方便对比：

```
------ Output (original) ------
Robot says: hello world
Robot says: the original message
Robot says: 1
Robot says: 2
Robot says: 3

------ Output (substituted) ------
DummySaySomething: hello world
DummyAction substituting node with fullPath(): mysub/action_subA
DummyAction substituting node with fullPath(): mysub/action_subB
Robot says: message SUBSTITUTED
```
