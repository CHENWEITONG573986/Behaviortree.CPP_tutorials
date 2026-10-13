// ============================================================================
// topic_nodes.hpp
// ----------------------------------------------------------------------------
// 第三种 ROS 通信方式：Topic（话题，发布/订阅）。
//
// Topic 和 Action/Service 的区别：
//   - Topic 是“广播”，谁都能订阅，消息是**单向、持续**的流（传感器数据最典型）；
//   - 订阅方不“请求”，只是被动接收最新消息。
//
// 官方提供了两个封装基类：
//   BT::RosTopicPubNode<T>  —— 把“发布一条话题消息”变成节点（继承自 ConditionNode）
//   BT::RosTopicSubNode<T>  —— 把“读取最新话题消息”变成节点
//
// 注意它们和 Action/Service 节点不太一样：
//   - Pub 节点：实现 setMessage()，tick 时发布一次，返回 SUCCESS/FAILURE。
//   - Sub 节点：实现 onTick(last_msg)，读“自上次 tick 以来最新的一条消息”，
//     必须返回 SUCCESS 或 FAILURE（不能返回 RUNNING）。
//   - 若还没有收到任何消息，传给 onTick 的 last_msg 是空指针。
// ============================================================================
#pragma once

#include <memory>
#include <string>

#include <behaviortree_ros2/bt_topic_pub_node.hpp>
#include <behaviortree_ros2/bt_topic_sub_node.hpp>

#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"

using namespace BT;

// ----------------------------------------------------------------------------
// CheckTemperature：订阅 /temperature（Float64），判断温度是否在 [min, max] 内。
//   在范围内 -> SUCCESS，超出范围 -> FAILURE
//   还没收到消息 -> FAILURE（空指针）
// ----------------------------------------------------------------------------
class CheckTemperature : public RosTopicSubNode<std_msgs::msg::Float64>
{
public:
  CheckTemperature(const std::string& name, const NodeConfig& conf,
                   const RosNodeParams& params)
    : RosTopicSubNode<std_msgs::msg::Float64>(name, conf, params)
  {}

  // 基类自带 topic_name 端口，用 providedBasicPorts 合并进来。
  // 再增加 min / max 两个输入端口（带默认值）。
  static BT::PortsList providedPorts()
  {
    return providedBasicPorts({
        InputPort<double>("min", 0.0, "允许的最低温度"),
        InputPort<double>("max", 100.0, "允许的最高温度"),
    });
  }

  // tick 时被调用，last_msg 是“最新收到的一条消息”；没有则为空指针。
  NodeStatus onTick(const std::shared_ptr<std_msgs::msg::Float64>& last_msg) override
  {
    if(!last_msg)
    {
      // 注意：onTick 只能返回 SUCCESS 或 FAILURE。没收到消息时一般返回 FAILURE，
      // 由上层（或执行器的重试循环）稍后再试。
      RCLCPP_WARN(logger(), "[CheckTemperature] 还没有收到温度消息");
      return NodeStatus::FAILURE;
    }

    double lo = 0.0, hi = 100.0;
    getInput("min", lo);
    getInput("max", hi);

    RCLCPP_INFO(logger(), "[CheckTemperature] 收到温度 %.2f（允许 %.1f ~ %.1f）",
                last_msg->data, lo, hi);

    return (last_msg->data >= lo && last_msg->data <= hi) ? NodeStatus::SUCCESS
                                                          : NodeStatus::FAILURE;
  }
};

// ----------------------------------------------------------------------------
// PublishCommand：往 /robot_command（String）发布一条文本指令。
// tick 时发布一次，setMessage() 返回 false 则节点返回 FAILURE、不发送。
// ----------------------------------------------------------------------------
class PublishCommand : public RosTopicPubNode<std_msgs::msg::String>
{
public:
  PublishCommand(const std::string& name, const NodeConfig& conf,
                 const RosNodeParams& params)
    : RosTopicPubNode<std_msgs::msg::String>(name, conf, params)
  {}

  static BT::PortsList providedPorts()
  {
    return providedBasicPorts({ InputPort<std::string>("command", "要发布的文本指令") });
  }

  // 填好要发布的消息内容；返回 true 表示可以发送。
  bool setMessage(std_msgs::msg::String& msg) override
  {
    if(!getInput("command", msg.data))
    {
      return false;
    }
    RCLCPP_INFO(node_->get_logger(), "[PublishCommand] 发布指令: %s", msg.data.c_str());
    return true;
  }
};
