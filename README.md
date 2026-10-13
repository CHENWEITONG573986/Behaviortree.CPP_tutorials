# BehaviorTree.CPP v4 教程合集（官方文档 01–16）

这个目录把 [behaviortree.dev 官方文档教程](https://www.behaviortree.dev/docs/intro) 的
**16 篇**（Basic 01–11 + Advanced 12–16）整理成了一个个**可独立编译运行**的目录，
源码对应官方仓库 `examples/t01 … t16`，并加了中文注释。

## 环境要求

| 项 | 说明 |
|----|------|
| 库 | BehaviorTree.CPP **v4**（本机通过 vcpkg 安装，4.10.0） |
| vcpkg 根目录 | `/home/rpp/vcpkg`（下文用 `$VCPKG_ROOT` 表示） |
| 编译器 | 支持 C++17 的编译器（本机 g++ 11） |
| CMake | ≥ 3.16 |

> ⚠️ 注意：`/opt/ros/humble` 里还有一套 **v3**（`behaviortree_cpp_v3`），
> 那是 ROS 自带的，和这里的 v4 **不是一回事**，不要混用。

## 目录结构

```
behaviortree_demo/
├── README.md                # 本文件
├── build_all.sh             # 一键编译（可选）
├── sample_nodes/            # 多个教程共用的示例节点库
│   ├── dummy_nodes.{h,cpp}
│   ├── movebase_node.{h,cpp}
│   └── crossdoor_nodes.{h,cpp}
├── tutorials01/ … tutorials16/
│   ├── main.cpp             # 教程源码（含中文注释）
│   ├── README.md            # 本节说明 + 编译运行 + 期望输出
│   ├── CMakeLists.txt
│   └── *.xml                # 需要 XML 的教程才有
```

每个 `tutorialsNN` 都可以**单独**配置、编译、运行；需要复用节点的教程通过
相对路径 `../sample_nodes` 引入，所以请保留当前的目录层级。

## 教程索引

| 目录 | 主题 | 官方文档 |
|------|------|----------|
| [tutorials01](tutorials01/README.md) | 你的第一棵行为树 | [01](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_01_first_tree) |
| [tutorials02](tutorials02/README.md) | 黑板与端口 | [02](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_02_basic_ports) |
| [tutorials03](tutorials03/README.md) | 带自定义类型的端口 | [03](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_03_generic_ports) |
| [tutorials04](tutorials04/README.md) | 反应式行为（Sequence vs ReactiveSequence） | [04](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_04_sequence) |
| [tutorials05](tutorials05/README.md) | 使用子树 | [05](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_05_subtrees) |
| [tutorials06](tutorials06/README.md) | 子树端口重映射 | [06](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_06_subtree_ports) |
| [tutorials07](tutorials07/README.md) | 使用多个 XML 文件 | [07](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_07_multiple_xml) |
| [tutorials08](tutorials08/README.md) | 给节点传额外参数 | [08](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_08_additional_args) |
| [tutorials09](tutorials09/README.md) | 脚本（Scripting） | [09](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_09_scripting) |
| [tutorials10](tutorials10/README.md) | 日志与观察者 | [10](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_10_observer) |
| [tutorials11](tutorials11/README.md) | 连接到 Groot2（需启用 groot） | [11](https://www.behaviortree.dev/docs/tutorial-basics/tutorial_11_groot2) |
| [tutorials12](tutorials12/README.md) | 端口的默认值 | [12](https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_12_default_ports) |
| [tutorials13](tutorials13/README.md) | 按引用访问端口内容 | [13](https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_13_blackboard_reference) |
| [tutorials14](tutorials14/README.md) | 子树模型与自动重映射 | [14](https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_14_subtree_model) |
| [tutorials15](tutorials15/README.md) | Mock 与节点替换 | [15](https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_15_replace_rules) |
| [tutorials16](tutorials16/README.md) | 全局黑板惯用法 | [16](https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_16_global_blackboard) |
| [ros2_integration](ros2_integration/README.md) | ROS 2 集成（官方 BehaviorTree.ROS2，用 colcon 构建） | [Integration with ROS2](https://www.behaviortree.dev/docs/ros2_integration) |

## 编译运行某一个教程

vcpkg 的配置已经写进每个 `CMakeLists.txt`，**直接 `cmake` 即可**，不用再传
`-DCMAKE_TOOLCHAIN_FILE`：

```bash
cd tutorials03
cmake -S . -B build
cmake --build build
./build/bt_tutorial_03
```

> 每个 `CMakeLists.txt` 在 `project()` 之前会自动把 vcpkg 工具链设好：
> 优先读环境变量 `VCPKG_ROOT`，没有则回退到本机默认路径 `/home/rpp/vcpkg`。
> 换机器时设一下 `VCPKG_ROOT` 即可，无需改文件。
>
> 某些教程（如 01、07）用相对路径读取 XML，请在对应教程目录里运行可执行文件。

## 一键编译 / 运行

```bash
./build_all.sh          # 编译并运行全部（教程 11 会被自动跳过）
./build_all.sh 05       # 只处理 tutorials05
```

## 关于教程 11（Groot2）

`Groot2Publisher` 需要 behaviortree_cpp 在编译时启用 Groot2 接口
（vcpkg feature `groot`，即 `BTCPP_GROOT_INTERFACE=ON`）。本机已启用
（`vcpkg install "behaviortree-cpp[groot]" --recurse`），所以 `tutorials11` 可以正常编译；
未启用的机器上它的 CMake 会自动探测、打印警告并跳过，其它教程不受影响。
详见 [tutorials11/README.md](tutorials11/README.md)。

## ROS 2 集成（额外章节）

除 16 篇编号教程外，官方文档还有一章 **Integration with ROS2**，介绍官方的
[BehaviorTree.ROS2](https://github.com/BehaviorTree/BehaviorTree.ROS2) 封装库
（把 ROS 2 的 Action / Service / Topic 包装成 TreeNode，并提供一个 Task Planner 执行器）。

这一章与前面不同：用 **colcon** 构建，并且需要 **BT.CPP 4.6+ 的 ROS 包**
`ros-humble-behaviortree-cpp`（vcpkg 那版导出的目标名不匹配，不能用）。
完整说明和一键脚本见 [ros2_integration/README.md](ros2_integration/README.md)：

```bash
cd ros2_integration
./setup.sh     # 准备 BT.CPP v4 + 克隆官方仓库（缺依赖时自动下载 deb，免 sudo）
./build.sh     # colcon build
./run_demo.sh  # 启动执行器并发送一个行为树目标
```
