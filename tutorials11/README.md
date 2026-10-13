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

## ⚠️ 本机前提：需要启用 Groot2 接口

`Groot2Publisher` 只有在 behaviortree_cpp 编译时打开 `BTCPP_GROOT_INTERFACE` 才存在。
**本机 vcpkg 装的 behaviortree_cpp 没启用它**（`BTCPP_GROOT_INTERFACE=OFF`），
所以本目录的 CMake 会打印一条警告并**跳过**编译（其余教程不受影响）。

如果想启用，用 vcpkg 的 `groot` feature 重新安装：

```bash
$VCPKG_ROOT/vcpkg install "behaviortree-cpp[groot]"
# 例：/home/rpp/vcpkg/vcpkg install "behaviortree-cpp[groot]"
```

它会额外编译 `cppzmq`/`libzmq`。装好后重新配置本目录即可：

```bash
cd tutorials11
cmake -S . -B build
cmake --build build
./build/bt_tutorial_11
```

然后用 Groot2（<https://www.behaviortree.dev/groot>）连接到 `localhost:1667`。

## 说明

官方示例的循环是 `while(true)`（一直等待 Groot2 连接）。为了让程序能自动结束，
这里的循环改成了固定 3 次。
