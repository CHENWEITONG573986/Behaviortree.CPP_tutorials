# 教程 02：黑板与端口

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_02_basic_ports>

## 本节要点

- **端口**是节点之间交换信息的机制，底层是共享的 key/value 存储 **Blackboard（黑板）**。
- 输入端口 ≈ 函数参数；输出端口 ≈ 返回值。类型和数量在编译期静态确定。
- 带端口的节点必须实现静态方法 `providedPorts()`。
- `SimpleActionNode` 自己没有 `providedPorts()`，注册时要把 `PortsList` 显式传进去。
- XML 中输入端口有两种写法：
  - 字面量：`message="hello"`
  - 指向黑板条目：`message="{the_answer}"`

## 这棵树

```
Sequence(root)
├── SaySomething   message="hello"
├── SaySomething2  message="this works too"
├── ThinkWhatToSay text="{the_answer}"     # 往黑板写 the_answer
└── SaySomething2  message="{the_answer}"  # 从黑板读 the_answer
```

## 编译与运行

```bash
cd tutorials02
cmake -S . -B build
cmake --build build
./build/bt_tutorial_02
```

## 期望输出

```
Robot says: hello
Robot says: this works too
Robot says: The answer is 42
```

> 注意只打印了 3 行：`ThinkWhatToSay` 本身不打印，它只是往黑板写值。
