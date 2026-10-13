// ============================================================================
// add_two_ints_service.hpp
// ----------------------------------------------------------------------------
// 本节重点之二：把 ROS 2 的 Service（服务）包装成一个行为树节点。
//
// Service 和 Action 的区别（一句话）：
//   - Service：请求-应答，一问一答，是阻塞式的短操作（适合“查个值/算个数”）。
//   - Action：有目标、进度、结果，还支持取消，适合耗时较长的任务。
//
// 继承 BT::RosServiceNode<ServiceT> 后必须实现的回调：
//   1) setRequest(Request::SharedPtr&)          —— 填好要发出去的请求
//   2) onResponseReceived(Response::SharedPtr&) —— 收到应答（返回 SUCCESS/FAILURE）
//   3) onFailure(ServiceNodeErrorCode)          —— 通信出错
// ============================================================================
#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <behaviortree_ros2/bt_service_node.hpp>
#include "example_interfaces/srv/add_two_ints.hpp"

using namespace BT;

class AddTwoIntsService : public RosServiceNode<example_interfaces::srv::AddTwoInts>
{
public:
  AddTwoIntsService(const std::string& name, const NodeConfig& conf,
                    const RosNodeParams& params)
    : RosServiceNode<example_interfaces::srv::AddTwoInts>(name, conf, params)
  {}

  // --------------------------------------------------------------------------
  // 声明端口：
  //   基类提供 action_name（这里实际是“服务名”，端口名沿用基类的 action_name）。
  //   我们新增：A、B 两个输入 + sum 一个输出，都用 int64_t（和服务字段一致）。
  // --------------------------------------------------------------------------
  static BT::PortsList providedPorts()
  {
    return providedBasicPorts({
        InputPort<int64_t>("A", "加数 A"),
        InputPort<int64_t>("B", "加数 B"),
        OutputPort<int64_t>("sum", "A + B 的结果"),
    });
  }

  // --------------------------------------------------------------------------
  // 回调 1：setRequest
  // 把端口里的 A、B 填进请求。返回 true 表示可以发送。
  // --------------------------------------------------------------------------
  bool setRequest(Request::SharedPtr& request) override
  {
    if(!getInput("A", request->a) || !getInput("B", request->b))
    {
      return false;
    }
    return true;
  }

  // --------------------------------------------------------------------------
  // 回调 2：onResponseReceived
  // 收到应答。这里把结果写到输出端口 sum，这样 XML 里 sum="{sum}" 就能把它
  // 存进黑板，供后面的节点读取。
  // 注意：setOutput("sum", ...) 的端口名是输入端口之外另外声明的 OutputPort。
  // --------------------------------------------------------------------------
  BT::NodeStatus onResponseReceived(const Response::SharedPtr& response) override
  {
    setOutput("sum", response->sum);
    RCLCPP_INFO(logger(), "[AddTwoInts] 服务返回: sum = %ld", (long)response->sum);
    return BT::NodeStatus::SUCCESS;
  }

  // --------------------------------------------------------------------------
  // 回调 3：onFailure
  // 服务不可达 / 超时 / 请求非法等。
  // --------------------------------------------------------------------------
  BT::NodeStatus onFailure(ServiceNodeErrorCode error) override
  {
    RCLCPP_ERROR(logger(), "[AddTwoInts] 服务调用失败: %s", BT::toStr(error));
    return BT::NodeStatus::FAILURE;
  }
};
