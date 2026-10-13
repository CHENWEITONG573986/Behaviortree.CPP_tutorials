// ============================================================================
// add_two_ints_server.cpp
// ----------------------------------------------------------------------------
// 一个最普通的 ROS 2 Service 服务端（用官方教程里的 example_interfaces/AddTwoInts）。
// 收到 {a, b}，把 a+b 作为响应返回。服务名固定为 "add_two_ints"。
//
// Service 比 Action 简单：没有 feedback、没有取消，就是一问一答。
// ============================================================================
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

using AddTwoInts = example_interfaces::srv::AddTwoInts;

// 收到请求时的处理函数：request 是请求，response 是要填的应答
void handleAdd(const std::shared_ptr<AddTwoInts::Request> request,
               std::shared_ptr<AddTwoInts::Response> response)
{
  response->sum = request->a + request->b;
  RCLCPP_INFO(rclcpp::get_logger("add_two_ints_server"), "收到请求: %ld + %ld = %ld",
              (long)request->a, (long)request->b, (long)response->sum);
}

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);

  auto node = rclcpp::Node::make_shared("add_two_ints_server");
  // 创建服务，服务名 "add_two_ints"
  auto service = node->create_service<AddTwoInts>("add_two_ints", &handleAdd);

  RCLCPP_INFO(node->get_logger(), "AddTwoInts 服务端已启动，服务名: add_two_ints");

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
