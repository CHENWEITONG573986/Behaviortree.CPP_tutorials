# 教程 11：连接到 Groot2

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_11_groot2>

## 本节要点

- **Groot2** 是官方的可视化 IDE：可以编辑、监视、交互运行中的行为树。
- `BT::Groot2Publisher publisher(tree, port);` 打开一个 TCP 端口（示例是 1667），
  Groot2 连上来就能实时看到节点状态。
- 用 `BT::WriteTreeToXML(tree, ...)` 导出树结构，`BT::writeTreeNodesModelXML(factory)`
  导出节点模型（Groot2 编辑节点要用）。
- `BT::RegisterJsonDefinition<T>()` 让 Groot2 能显示你的自定义类型。
- `FileLogger2` / `MinitraceLogger` 把状态转移写进文件（`.btlog` / `.json`）。

## Groot2 接口依赖

`Groot2Publisher` 只有在 behaviortree_cpp 编译时打开 `BTCPP_GROOT_INTERFACE` 才存在。
本目录的 CMake 会自动探测：

- **已启用** → 正常编译出 `bt_tutorial_11`；
- **未启用** → 打印一条警告并跳过编译（其余教程不受影响）。

本机已经启用（用 vcpkg 的 `groot` feature 重装过）：

```bash
$VCPKG_ROOT/vcpkg install "behaviortree-cpp[groot]" --recurse
# 例：/home/rpp/vcpkg/vcpkg install "behaviortree-cpp[groot]" --recurse
```

它会额外编译 `cppzmq`/`libzmq`。配置并运行：

```bash
cd tutorials11
cmake -S . -B build
cmake --build build
./build/bt_tutorial_11
```

程序会监听端口 1667，然后在 Groot2（<https://www.behaviortree.dev/groot>）里
连接到 `localhost:1667`。

## 说明

和官方示例一致，这里是 `while(true)` **一直循环运行**，等待/服务 Groot2 连接；
想停止时在终端按 **Ctrl-C**。
