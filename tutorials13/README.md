# 教程 13：按引用访问端口内容

官方文档：<https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_13_blackboard_reference>

## 本节要点

- 默认情况下端口是**值语义**（拷贝）。要修改黑板里的容器（如 `std::vector`）时，
  频繁拷贝代价大，这时用 **`getLockedPortContent("port")`**。
- 它返回一个“加锁”的指针；只要该指针有效，对黑板条目的访问就是**互斥、线程安全**的。
- `any_ptr->empty()` 判断条目是否已初始化；`any_ptr.assign(x)` 写入；
  `any_ptr->castPtr<T>()` 拿到具体类型的指针（类型不符会返回 `nullptr`）。
- 端口用 `BT::BidirectionalPort<T>` 声明（既能读也能写）。

## 编译与运行

```bash
cd tutorials13
cmake -S . -B build
cmake --build build
./build/bt_tutorial_13
```

## 期望输出

```
We created a new vector, containing value [3]
Value [5] pushed into the vector. New size: 2
Value [7] pushed into the vector. New size: 3
```
