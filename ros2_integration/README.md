# ROS 2 集成（官方 BehaviorTree.ROS2）

对应官方文档章节：**Integration with ROS2**
<https://www.behaviortree.dev/docs/ros2_integration>

官方提供了现成的封装库 **[BehaviorTree.ROS2](https://github.com/BehaviorTree/BehaviorTree.ROS2)**，
让你用很少的样板代码就能写出“跟 ROS 2 打交道的 TreeNode”——
把 ROS 的 **Action / Service / Topic** 包装成行为树节点。

> 注意：ROS 语境和 BT.CPP 语境里有两个词同名不同义，别混：
> - `TreeNode`（行为树节点） vs `rclcpp::Node`（ROS 节点）
> - `BT::Action`（行为树里的动作） vs `rclcpp_action`（ROS 的 Action）

## 推荐的系统架构

- 有一个**中心的 “Coordinator” 节点**用 BT.CPP 负责编排整个行为，官方文档称其为
  **Task Planner（任务规划器）**。
- 其它模块都是**面向服务（service-oriented）**的：只提供能力、不做决策，
  所有业务逻辑与决策都交给 Task Planner。

本目录跑的就是官方示例里的那个 Task Planner：`sample_bt_executor`
（`btcpp_ros2_samples`），它对外暴露一个 ROS Action（默认 `/behavior_server`），
收到目标后执行指定的行为树。

## ⚠️ 版本搭配（重要）

`behaviortree_ros2` 只兼容 **BT.CPP 4.6+**，而且它链接的 CMake 目标名是
**`behaviortree_cpp::behaviortree_cpp`**。

| 来源 | 导出目标名 | 能不能用于本教程 |
|------|-----------|------------------|
| `/opt/ros/humble` 的 `ros-humble-behaviortree-cpp`（v4.10） | `behaviortree_cpp::behaviortree_cpp` | ✅ 推荐 |
| 你的 vcpkg 版 v4.10（`sample_nodes` 那套教程用的） | `BT::behaviortree_cpp` | ❌ 名字对不上 |
| ROS 自带的 `behaviortree_cpp_v3`（3.8） | `behaviortree_cpp_v3::behaviortree_cpp_v3` | ❌ 版本太低 |

所以本节**不用 vcpkg 的 v4**，而是用 ROS 源里的 `ros-humble-behaviortree-cpp`。

## 目录结构

```
ros2_integration/
├── README.md          # 本文件
├── env.sh             # 环境准备（被下面三个脚本 source）
├── setup.sh           # 准备 BT.CPP v4 + 克隆官方 BehaviorTree.ROS2
├── build.sh           # colcon build
├── run_demo.sh        # 启动执行器 + 发送一个行为树目标
├── src/               # (setup.sh 拉取) BehaviorTree.ROS2: 3 个包
│   └── BehaviorTree.ROS2/
│       ├── behaviortree_ros2/        # 封装库本体
│       ├── btcpp_ros2_interfaces/    # 自定义 Action/Msg/Srv
│       └── btcpp_ros2_samples/       # 官方示例（执行器 + 各种客户端/服务端）
├── prefix/            # (setup.sh 生成) 未装 apt 包时的本地 BT.CPP v4（免 sudo）
├── build/ install/ log/   # colcon 产物
```

## 使用步骤

```bash
cd ros2_integration
./setup.sh        # 准备 BT.CPP v4 + 拉取官方仓库
./build.sh        # colcon build
./run_demo.sh                 # 默认执行 CrossDoor 行为树
./run_demo.sh SleepActionSample   # 换一棵树
```

`setup.sh` 会尽量少麻烦你：如果系统已装 `ros-humble-behaviortree-cpp` 就直接用；
否则**下载对应 deb 解包到 `prefix/`**（不需要 sudo）。
若你想更规范，可以自己装一次：

```bash
sudo apt install ros-humble-behaviortree-cpp
```

## 期望输出

`run_demo.sh` 会启动执行器，然后发送一个 `ExecuteTree` 目标：

```
[bt_action_server]: Starting Action Server: behavior_server
[bt_action_server]: Loaded ROS Plugin: libcrossdoor_nodes_dyn.so
[bt_action_server]: Loaded ROS Plugin: libdummy_nodes_dyn.so
[bt_action_server]: Loaded ROS Plugin: libmovebase_node_dyn.so
[bt_action_server]: Loaded ROS Plugin: libsleep_plugin.so
[bt_action_server]: Loaded BehaviorTree: cross_door.xml
...
Received goal request to execute Behavior Tree: CrossDoor
...
Goal finished with status: SUCCEEDED
```

## 手册里那两类封装（本节跑的后台就是它们）

文档展示了两种常见封装，源码分别在
`src/BehaviorTree.ROS2/behaviortree_ros2/include/behaviortree_ros2/`：

1. **Action 客户端** —— 继承 `BT::RosActionNode<ActionT>`：
   重写 `setGoal()` / `onFeedback()` / `onResultReceived()` / `onFailure()`。
   文档里以 Fibonacci 为例（见 `bt_action_node.hpp`、示例 `btcpp_ros2_samples`）。
2. **Service 客户端** —— 继承 `BT::RosServiceNode<ServiceT>`：
   重写 `setRequest()` / `onResponseReceived()` / `onFailure()`。
   文档里以 `AddTwoInts` 为例。

注册时要传入 ROS 节点的参数：

```cpp
RosNodeParams params;
params.nh = node;                       // std::shared_ptr<rclcpp::Node>
params.default_port_value = "fibonacci";
factory.registerNodeType<FibonacciAction>("Fibonacci", params);
```

## 和 Groot2 的关系

`sample_bt_executor` 的配置里默认 `groot2_port: 1667`
（见 `src/BehaviorTree.ROS2/btcpp_ros2_samples/config/sample_bt_executor.yaml`），
所以跑起来后可以直接用 Groot2 连接 `localhost:1667` 实时观察。
