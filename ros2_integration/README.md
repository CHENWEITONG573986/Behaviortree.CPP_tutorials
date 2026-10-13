# ROS 2 集成教学（BT.CPP v4 + ROS 2 Humble）

官方文档：<https://www.behaviortree.dev/docs/ros2_integration>
官方封装库：<https://github.com/BehaviorTree/BehaviorTree.ROS2>

本节带你从零理解并跑通「把 ROS 2 的能力接进行为树」这件事。
读完之后你应该能自己写出“调用 ROS Action / Service 的行为树节点”。

---

## 0. 先分清三个概念（很重要）

学之前先把几个容易混淆的词摆正：

| 概念 | 它是什么 | 一句话理解 |
|------|----------|-----------|
| **行为树 (BT)** | 一套任务编排逻辑 | “先做 A，再做 B，失败了就做 C” |
| **TreeNode** | 行为树里的一个节点 | 行为树的“零件”，`tick()` 时干活、返回 `SUCCESS/FAILURE/RUNNING` |
| **ROS 节点 (`rclcpp::Node`)** | ROS 2 里的通信实体 | 收发 topic/service/action 的“通信账户” |

> ⚠️ 官方文档专门提醒的两个同名不同义词：
> - `TreeNode`（行为树节点） **≠** `rclcpp::Node`（ROS 节点）
> - `BT::Action`（行为树里的“动作节点”） **≠** `rclcpp_action`（ROS 的 Action 机制）
>
> 看代码时别被名字骗了。

---

## 1. 为什么要“集成”，以及怎么组织系统

行为树本身是 ROS 无关的纯 C++ 库。但机器人上事情大多要通过 ROS 去做
（导航、开阀、抓取……都是 Action / Service / Topic）。于是我们需要**把 ROS 通信包装成 TreeNode**。

官方推荐的系统架构是：

```
        ┌─────────────────────────┐
        │  Task Planner（任务规划器）│   ← 用 BT.CPP 实现，是“大脑”
        │  一个中心 ROS 节点         │
        └───────────┬─────────────┘
     调用 Action/Service/Topic
   ┌────────────────┼────────────────┐
   ▼                ▼                ▼
 导航服务         机械臂服务        传感器话题
（只提供能力，不做决策——所谓“面向服务”）
```

要点：**所有判断和决策都集中在 Task Planner（行为树）里**，其它模块只管“能不能做”，
不做“该不该做”。这样逻辑只有一个地方，好维护、好调试。

---

## 2. 环境准备

### 2.1 用哪个 BT.CPP？（**最容易踩的坑**）

`behaviortree_ros2` 只兼容 **BT.CPP 4.6+**，而且它链接的 CMake 目标名必须是
**`behaviortree_cpp::behaviortree_cpp`**。对照一下：

| BT.CPP 来源 | 导出目标名 | 能不能用 |
|---|---|---|
| ROS 源 `ros-humble-behaviortree-cpp`（v4.10） | `behaviortree_cpp::behaviortree_cpp` | ✅ **本节就用它** |
| 前面 16 篇用的 vcpkg 版 v4.10 | `BT::behaviortree_cpp` | ❌ 名字对不上 |
| ROS 自带的 `behaviortree_cpp_v3`（3.8） | `behaviortree_cpp_v3::...` | ❌ 版本太低 |

所以本节**不用 vcpkg 那套**，而是用 ROS 源里的 v4。**推荐安装方式（最规范）：**

```bash
sudo apt install ros-humble-behaviortree-cpp
```

`setup.sh` 会自动判断：**装了就用系统的**；没装就下载对应 deb 解包到本目录 `prefix/`
（不需要 sudo，方便在没有 root 的机器上也能学）。

### 2.2 获取官方封装库

```bash
cd ros2_integration
./setup.sh     # 准备 BT.CPP v4 + 克隆官方 BehaviorTree.ROS2
./build.sh     # colcon 构建
```

`setup.sh` 会把官方仓库（`behaviortree_ros2` / `btcpp_ros2_interfaces` / `btcpp_ros2_samples`）
克隆到 `src/BehaviorTree.ROS2/`，并固定到一个**已验证过的提交**，保证可复现。

---

## 3. 先跑两个现成例子感受一下

### 3.1 官方 samples（看“标准执行器”长什么样）

```bash
./run_demo.sh                     # 执行 CrossDoor 行为树
./run_demo.sh SleepActionSample   # 换成 SleepActionSample
```

它会启动官方那个 `sample_bt_executor`——一个标准的 **Task Planner 执行器**，
对外暴露一个 ROS Action `/behavior_server`，收到目标后执行指定的行为树：

```
[bt_action_server]: Starting Action Server: behavior_server
[bt_action_server]: Loaded ROS Plugin: libcrossdoor_nodes_dyn.so
...
Received goal request to execute Behavior Tree: CrossDoor
Goal finished with status: SUCCEEDED
```

### 3.2 本节教学示例（**重点，代码都是我们写的、注释详尽**）

```bash
./run_tutorial.sh
```

它会：启动一个倒计时 Action 服务端 + 一个加法 Service 服务端，然后运行行为树执行器。
预期输出：

```
[PrintValue] === 开始执行行为树 ===
[Countdown] 进度反馈: 4
[Countdown] 进度反馈: 3
[Countdown] 进度反馈: 2
[Countdown] 进度反馈: 1
[Countdown] 结果: 倒计时完成
[PrintValue] 倒计时完成，下面调用 ROS 服务做一次加法
[AddTwoInts] 服务返回: sum = 7
[PrintNumber] 7
[PrintValue] === 行为树执行结束 ===
行为树执行结束，最终状态: SUCCESS
```

---

## 4. 手把手：这个教学示例是怎么写出来的

代码在 `src/bt_ros2_tutorial/`，结构如下：

```
bt_ros2_tutorial/
├── package.xml / CMakeLists.txt
├── action/Countdown.action                     # ① 自定义 Action 接口
├── behavior_trees/countdown_tree.xml           # ⑥ 行为树定义
├── launch/tutorial.launch.xml                  # 一把启动所有节点
├── include/bt_ros2_tutorial/
│   ├── countdown_action.hpp                     # ③ ROS Action -> TreeNode
│   ├── add_two_ints_service.hpp                 # ④ ROS Service -> TreeNode
│   └── print_nodes.hpp                          # ⑤ 普通节点（不涉及 ROS）
└── src/
    ├── countdown_server.cpp                     # ② Action 服务端
    ├── add_two_ints_server.cpp                  # ② Service 服务端
    └── tutorial_executor.cpp                    # ⑦ 执行器（main）
```

### ① 定义自己的 Action 接口（`action/Countdown.action`）

ROS 2 的 Action 用「三短横线」把三段分开：目标 / 结果 / 反馈。

```
int32 count      # Goal：从 count 开始倒数
---
string message   # Result：结束后的一句话
---
int32 current    # Feedback：过程中的当前值
```

CMake 里用 `rosidl_generate_interfaces` 生成 C++ 头文件（会得到
`bt_ros2_tutorial/action/countdown.hpp`）。

### ② 写 Action 服务端（`src/countdown_server.cpp`）

这就是个**普通 rclcpp 节点**，和行为树无关。一个 Action 服务端要实现三个回调：

| 回调 | 什么时候调用 | 你要做什么 |
|------|--------------|-----------|
| `handle_goal` | 收到目标 | 返回 `ACCEPT_AND_EXECUTE`（接受）或 `REJECT` |
| `handle_cancel` | 收到取消请求 | 返回 `ACCEPT` / `REJECT` |
| `handle_accepted` | 目标被接受 | **把真正的执行放到单独线程**（别阻塞 executor） |

执行时用 `publish_feedback()` 发进度，最后 `succeed(result)` 或 `canceled(result)`。

### ③ 把 ROS Action 变成 TreeNode（`include/.../countdown_action.hpp`）

这是本节**最重要的知识点**。继承 `BT::RosActionNode<Countdown>` 后，
只需要填 4 个回调：

```cpp
class CountdownAction : public RosActionNode<Countdown>
{
  // 端口：必须用 providedBasicPorts 把基类的 action_name 一起合并进来
  static BT::PortsList providedPorts()
  { return providedBasicPorts({ InputPort<int>("order", "倒数的起始数字") }); }

  // 1) 第一次 tick：把端口数据填进 goal
  bool setGoal(Goal& goal) override { return getInput("order", goal.count); }

  // 2) 收到进度：一般返回 RUNNING，继续等
  BT::NodeStatus onFeedback(const std::shared_ptr<const Feedback> fb) override
  { /* 打印 fb->current */ return BT::NodeStatus::RUNNING; }

  // 3) 收到最终结果：由你决定算成功还是失败
  BT::NodeStatus onResultReceived(const WrappedResult& wr) override
  { /* 用 wr.result->message */ return BT::NodeStatus::SUCCESS; }

  // 4) 通信出错：服务器不可达 / 超时 / 被取消……
  BT::NodeStatus onFailure(ActionNodeErrorCode err) override
  { return BT::NodeStatus::FAILURE; }
};
```

**它的时间线**（帮你建立直觉）：

```
第 1 次 tick  ──► setGoal() → 异步发出 goal → 返回 RUNNING
后续每次 tick ──► 收到 feedback → onFeedback()  （返回 RUNNING）
              └► 收到 result   → onResultReceived()（返回 SUCCESS/FAILURE）
被 halt()     ──► 框架自动向服务端发 cancel 请求
```

> **重点**：`RosActionNode` 的 `tick()` **内部自带 spin**（它有自己的
> callback group 和 executor），所以你的 `main()` 里**不用**手动 `spin`，
> 直接 `tree.tickWhileRunning()` 就行。这是很多人第一次写会搞错的地方。

### ④ 把 ROS Service 变成 TreeNode（`include/.../add_two_ints_service.hpp`）

Service 比 Action 简单（一问一答，没有进度和取消）。继承 `BT::RosServiceNode<T>`
只要 3 个回调：

```cpp
class AddTwoIntsService : public RosServiceNode<example_interfaces::srv::AddTwoInts>
{
  static BT::PortsList providedPorts()
  {
    return providedBasicPorts({
      InputPort<int64_t>("A"), InputPort<int64_t>("B"),
      OutputPort<int64_t>("sum"),          // 结果写回黑板
    });
  }

  // 1) 填请求
  bool setRequest(Request::SharedPtr& req) override
  { return getInput("A", req->a) && getInput("B", req->b); }

  // 2) 收到应答：把结果写到输出端口
  BT::NodeStatus onResponseReceived(const Response::SharedPtr& res) override
  { setOutput("sum", res->sum); return BT::NodeStatus::SUCCESS; }

  // 3) 出错
  BT::NodeStatus onFailure(ServiceNodeErrorCode err) override
  { return BT::NodeStatus::FAILURE; }
};
```

> 注意端口类型用 `int64_t`，和 `AddTwoInts` 的字段一致：**黑板是强类型的**，
> 一边按 `int64` 存、一边按 `int` 读会因类型不匹配而失败。

### ⑤ 普通节点：证明“混用”（`include/.../print_nodes.hpp`）

`PrintValue` / `PrintNumber` 是最普通的 `BT::SyncActionNode`，跟 ROS 毫无关系，
放在这里是为了说明：**普通节点和 ROS 节点可以混在同一棵树里**——行为树只是编排逻辑，
用不用 ROS 由每个节点自己决定。

### ⑥ 行为树 XML（`behavior_trees/countdown_tree.xml`）

```xml
<root BTCPP_format="4">
  <BehaviorTree ID="MainTree">
    <Sequence>
      <PrintValue  message="=== 开始执行行为树 ===" />
      <Countdown   order="5" />
      <PrintValue  message="倒计时完成，下面调用 ROS 服务做一次加法" />
      <AddTwoInts  A="3" B="4" sum="{sum}" />
      <PrintNumber val="{sum}" />
      <PrintValue  message="=== 行为树执行结束 ===" />
    </Sequence>
  </BehaviorTree>
</root>
```

- `<Countdown order="5"/>` 没写 `action_name` → 用注册时的默认值 `"countdown"`。
- `sum="{sum}"` 把输出端口写进黑板；下一行 `val="{sum}"` 再读出来。

### ⑦ 执行器（`src/tutorial_executor.cpp`）

```cpp
rclcpp::init(argc, argv);
auto node = std::make_shared<rclcpp::Node>("bt_tutorial_executor");   // ROS 通信实体
BT::BehaviorTreeFactory factory;

// 普通节点
factory.registerNodeType<PrintValue>("PrintValue");
factory.registerNodeType<PrintNumber>("PrintNumber");

// ROS 节点：通过 RosNodeParams 把 ROS 节点句柄和默认服务名传进去
BT::RosNodeParams action_params;
action_params.nh = node;
action_params.default_port_value = "countdown";
factory.registerNodeType<CountdownAction>("Countdown", action_params);

BT::RosNodeParams service_params;
service_params.nh = node;
service_params.default_port_value = "add_two_ints";
factory.registerNodeType<AddTwoIntsService>("AddTwoInts", service_params);

auto tree = factory.createTreeFromFile(xml_path);
tree.tickWhileRunning();      // 不需要手动 spin，详见 ③ 的说明
```

`RosNodeParams` 是“TreeNode 和 ROS 世界之间的桥”：`nh` 是那个 ROS 节点，
`default_port_value` 是该节点默认连接的动作/服务名。

---

## 5. 运行与观察

```bash
./run_tutorial.sh
```

想单独手动跑（更直观地体会 ROS）：

```bash
source /opt/ros/humble/setup.bash
source ros2_integration/install/setup.bash
ros2 launch bt_ros2_tutorial tutorial.launch.xml
```

或者分三个终端：

```bash
ros2 run bt_ros2_tutorial countdown_server
ros2 run bt_ros2_tutorial add_two_ints_server
ros2 run bt_ros2_tutorial tutorial_executor
```

用 ROS 命令行单独验证服务端：

```bash
ros2 action send_goal /countdown bt_ros2_tutorial/action/Countdown "{count: 3}" --feedback
ros2 service call /add_two_ints example_interfaces/srv/AddTwoInts "{a: 3, b: 4}"
```

---

## 6. 和 Groot2 结合

官方的 `sample_bt_executor` 默认打开了 Groot2 端口 **1667**
（见 `config/sample_bt_executor.yaml` 的 `groot2_port`）。
跑 `./run_demo.sh` 时用 Groot2 连 `localhost:1667`，就能实时看到节点状态变化。

---

## 7. 常见问题（FAQ）

**Q1：能不能用前面 16 篇的 vcpkg 版 BT.CPP？**
不能。它导出的目标是 `BT::behaviortree_cpp`，而 `behaviortree_ros2` 要的是
`behaviortree_cpp::behaviortree_cpp`。请用 `ros-humble-behaviortree-cpp`。

**Q2：`main()` 里要怎么 spin？**
不用手动 spin。`RosActionNode` / `RosServiceNode` 的 `tick()` 内部会处理。
（如果你用的是 `TreeExecutionServer`，它已经把 spin 也接好了。）

**Q3：为什么端口类型非得跟消息字段一致？**
黑板是强类型的。`int64_t` 存进去就用 `int64_t` 读，否则 `getInput` 会失败。

**Q4：`providedBasicPorts` 是干嘛的？**
基类自带 `action_name` 端口，`providedBasicPorts({...})` 负责把“基类端口 + 你的端口”
合并。不调用它，行为树会因为缺 `action_name` 而报错。

**Q5：`colcon` 报 “source directory ... does not contain CMakeLists.txt”？**
那是 `prefix/` 里的 BT.CPP 副本被当成了源码包。`build.sh` 已用 `--base-paths src` 限定，
手动构建时也请加上。

**Q6：`Address already in use`（端口 1667）？**
有上一次的残留进程。`run_*.sh` 会在启动前清理；手动排查可 `ss -ltnp | grep 1667`。

---

## 8. 命令速查 / 文件清单

```bash
cd ros2_integration
sudo apt install ros-humble-behaviortree-cpp    # （推荐）安装 ROS 版 BT.CPP v4
./setup.sh        # 准备 BT.CPP v4 + 克隆官方封装库
./build.sh        # colcon 构建全部包
./run_demo.sh     # 跑官方 samples（含标准执行器）
./run_tutorial.sh # 跑本节教学示例（我们写的代码）
ros2 launch bt_ros2_tutorial tutorial.launch.xml   # 手动启动教学示例
```

```
ros2_integration/
├── README.md            # 本教学文档
├── env.sh / setup.sh / build.sh
├── run_demo.sh          # 官方 samples
├── run_tutorial.sh      # 本节教学示例
└── src/
    ├── BehaviorTree.ROS2/     # setup.sh 克隆的官方库（不入库）
    └── bt_ros2_tutorial/      # 我们的教学代码（本仓库所有）
```

## 参考

- 官方文档：<https://www.behaviortree.dev/docs/ros2_integration>
- 官方封装库：<https://github.com/BehaviorTree/BehaviorTree.ROS2>
- 封装库文档：`src/BehaviorTree.ROS2/behaviortree_ros2/ros_behavior_wrappers.md`
- 执行器文档：`src/BehaviorTree.ROS2/behaviortree_ros2/tree_execution_server.md`
