# 教程 16：全局黑板惯用法

官方文档：<https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_16_global_blackboard>

## 本节要点

- 默认每棵子树黑板互相隔离，要共享数据得靠重映射。
- 从 **4.6** 起支持**全局黑板**：把它的 key 写成 **`{@键名}`**，
  任何子树里都能直接访问，不用重映射。
  - 前缀 `@` 表示“去层级最顶层的黑板里找这个条目”。
- 构造方式：
  ```cpp
  auto global_bb = BT::Blackboard::create();            // 全局黑板
  auto root_bb   = BT::Blackboard::create(global_bb);   // MainTree 的黑板，父级是全局
  auto tree = factory.createTree("MainTree", root_bb);
  ```

## 编译与运行

```bash
cd tutorials16
cmake -S . -B build
cmake --build build
./build/bt_tutorial_16
```

## 期望输出

```
[main_print] val: 1
[sub_print] val: 1
[While loop] value: 1 value_sqr: 1

[main_print] val: 2
[sub_print] val: 2
[While loop] value: 2 value_sqr: 4

[main_print] val: 3
[sub_print] val: 3
[While loop] value: 3 value_sqr: 9
```
