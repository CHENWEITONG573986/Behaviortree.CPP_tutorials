# 教程 06：子树端口重映射

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_06_subtree_ports>

## 本节要点

- 每棵子树都有自己的 **Blackboard**，默认与父树隔离（防止大型工程里命名冲突）。
- 子树的输入/输出端口，本质上是“内部端口”与“父树黑板条目”之间的**软链接**。
- 重映射语法：`<SubTree ID="MoveRobot" target="{move_goal}" result="{move_result}" />`
  - `target` / `result` 是子树 `MoveRobot` 的内部端口名；
  - `{move_goal}` / `{move_result}` 是父树黑板里的键。

## 这棵树

```
MainTree: Sequence
├── Script          code="move_goal='1;2;3'"
├── SubTree MoveRobot  target="{move_goal}"  result="{move_result}"
└── SaySomething    message="{move_result}"

MoveRobot: Fallback
├── Sequence
│   ├── MoveBase  goal="{target}"
│   └── Script    code="result:='goal reached'"
└── ForceFailure -> Script code="result:='error'"
```

## 编译与运行

```bash
cd tutorials06
cmake -S . -B build
cmake --build build
./build/bt_tutorial_06
```

## 期望输出（重点看两个黑板的 debugMessage）

```
------ First BB ------
move_result (std::string)
move_goal (Pose2D)

------ Second BB------
[result] remapped to port of parent tree [move_result]
[target] remapped to port of parent tree [move_goal]
```
