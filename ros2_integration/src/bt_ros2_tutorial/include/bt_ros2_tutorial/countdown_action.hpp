// ============================================================================
// countdown_action.hpp
// ----------------------------------------------------------------------------
// 本节的重点之一：把 ROS 2 的 Action（动作）包装成一个行为树节点。
//
// 为什么要“包装”？
//   ROS 的 Action 是异步、可取消的：客户端发目标(goal)、服务端回进度(feedback)、
//   最后回结果(result)。而行为树节点要能返回 RUNNING（表示“还没做完”）。
//   官方的 BT::RosActionNode<ActionT> 已经把这些样板（发目标、等反馈、收结果、
//   取消、超时……）都写好了，我们只要填几个回调即可。
//
// 继承 BT::RosActionNode<Countdown> 后必须实现的回调：
//   1) setGoal(Goal&)                       —— 第一次 tick 时，填好要发出去的 goal
//   2) onFeedback(feedback)                 —— 收到进度时（一般返回 RUNNING）
//   3) onResultReceived(WrappedResult&)     —— 收到最终结果时（返回 SUCCESS/FAILURE）
//   4) onFailure(ActionNodeErrorCode)       —— 通信出错时（返回 SUCCESS/FAILURE）
//
// 生命周期（重要）：
//   - 第一次 tick()：内部调用 setGoal()，把 goal 异步发给服务端，然后返回 RUNNING。
//   - 之后每次 tick()：内部会自己 spin ROS 回调；有 feedback 就调 onFeedback()，
//     有结果就调 onResultReceived()。所以我们的执行器不需要手动 spin。
//   - 如果该节点在 RUNNING 时被 halt()（比如 ReactiveSequence 里前面的条件变了），
//     框架会自动向服务端发 cancel 请求。
// ============================================================================
#pragma once

#include <memory>
#include <string>

// 官方封装：RosActionNode
#include <behaviortree_ros2/bt_action_node.hpp>
// 由 action/Countdown.action 自动生成的 C++ 类型
#include "bt_ros2_tutorial/action/countdown.hpp"

// 让下面少写几个 BT::。初学者建议保留，方便对照。
using namespace BT;

class CountdownAction : public RosActionNode<bt_ros2_tutorial::action::Countdown>
{
public:
  // 构造函数签名固定：(节点名, 节点配置, RosNodeParams)
  // RosNodeParams 里带着 ROS 节点句柄 `nh`，是它跟 ROS 通信的“凭据”。
  CountdownAction(const std::string& name, const NodeConfig& conf,
                  const RosNodeParams& params)
    : RosActionNode<bt_ros2_tutorial::action::Countdown>(name, conf, params)
  {}

  // --------------------------------------------------------------------------
  // 声明端口。
  // 基类自带一个输入端口 action_name（动作服务名）。
  // 必须用 providedBasicPorts({...}) 把“基类端口 + 本节点新增端口”合并返回，
  // 否则行为树会因为找不到 action_name 而报错。
  // 这里新增输入端口 order：倒计时的起始数字。
  // --------------------------------------------------------------------------
  static BT::PortsList providedPorts()
  {
    return providedBasicPorts({ InputPort<int>("order", "倒计时的起始数字") });
  }

  // --------------------------------------------------------------------------
  // 回调 1：setGoal
  // 第一次 tick 时被调用，用来把端口里的数据填进 goal。
  // 返回 true 表示“可以发送了”；返回 false 会被当成 INVALID_GOAL 失败。
  // --------------------------------------------------------------------------
  bool setGoal(Goal& goal) override
  {
    if(!getInput("order", goal.count))
    {
      // 端口没取到值（XML 里没写 order）
      return false;
    }
    return true;
  }

  // --------------------------------------------------------------------------
  // 回调 2：onFeedback
  // 每收到一次服务端的 feedback 就调用一次。通常返回 RUNNING（继续等）。
  // 也可以根据 feedback 的内容提前判定成功/失败（返回 SUCCESS / FAILURE）。
  // --------------------------------------------------------------------------
  BT::NodeStatus onFeedback(const std::shared_ptr<const Feedback> feedback) override
  {
    RCLCPP_INFO(logger(), "[Countdown] 进度反馈: %d", feedback->current);
    return BT::NodeStatus::RUNNING;
  }

  // --------------------------------------------------------------------------
  // 回调 3：onResultReceived
  // 收到最终 result 时调用，由我们决定这个节点整体算成功还是失败。
  // WrappedResult 里有 result 指针（对应我们 .action 里定义的 Result）。
  // --------------------------------------------------------------------------
  BT::NodeStatus onResultReceived(const WrappedResult& wr) override
  {
    RCLCPP_INFO(logger(), "[Countdown] 结果: %s", wr.result->message.c_str());
    return BT::NodeStatus::SUCCESS;
  }

  // --------------------------------------------------------------------------
  // 回调 4：onFailure
  // 通信层面出错时调用：服务器不可达、发目标超时、被取消、被 abort 等。
  // 返回 FAILURE 让行为树知道这个动作失败了（默认实现就是 FAILURE）。
  // --------------------------------------------------------------------------
  BT::NodeStatus onFailure(ActionNodeErrorCode error) override
  {
    RCLCPP_ERROR(logger(), "[Countdown] 动作失败: %s", BT::toStr(error));
    return BT::NodeStatus::FAILURE;
  }
};
