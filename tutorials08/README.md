# 教程 08：给节点传额外参数

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_08_additional_args>

## 本节要点

有些参数在编译期/部署期就确定、运行时不变，这时用“输入端口”反而麻烦。两种传参方式：

- **做法 A（构造函数参数）**：让节点的构造函数多接收几个参数，
  注册时直接传给 `registerNodeType<T>(...)`。
  想按引用传对象（如不可拷贝对象）要用 `std::ref` 包一层。
- **做法 B（延后初始化）**：正常构造节点，再用 `initialize(...)` 之类的方法在
  第一次 tick 之前初始化。用 `tree.applyVisitor()` 遍历节点来调用它。

## 编译与运行

```bash
cd tutorials08
cmake -S . -B build
cmake --build build
./build/bt_tutorial_08
```

## 期望输出

```
Action_A: 42 / hello world / 88
Action_B: 69 / interesting_value
```
