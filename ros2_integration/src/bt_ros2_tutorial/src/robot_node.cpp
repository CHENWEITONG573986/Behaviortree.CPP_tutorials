// ============================================================================
// robot_node.cpp
// ----------------------------------------------------------------------------
// 一个“模拟机器人”节点（普通 rclcpp 节点，跟行为树无关），做两件事：
//   1) 每 100ms 往 /temperature 发布一个模拟温度（Float64）；
//   2) 订阅 /robot_command（String），收到就打印出来。
//
// 它相当于真实系统里“提供传感器数据 / 执行指令”的设备侧。
// ============================================================================
#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "std_msgs/msg/string.hpp"

class RobotNode : public rclcpp::Node
{
public:
  RobotNode() : Node("mock_robot")
  {
    // 发布：温度，队列长度 10
    temp_pub_ = create_publisher<std_msgs::msg::Float64>("temperature", 10);

    // 订阅：指令。收到就打印（真实系统里这里会去驱动机器人）
    cmd_sub_ = create_subscription<std_msgs::msg::String>(
        "robot_command", 10,
        [this](const std_msgs::msg::String::SharedPtr msg) {
          RCLCPP_INFO(get_logger(), "收到指令: %s", msg->data.c_str());
        });

    // 每 100ms 发布一次温度
    timer_ = create_wall_timer(std::chrono::milliseconds(100),
                              [this]() { publishTemperature(); });

    RCLCPP_INFO(get_logger(), "模拟机器人已启动：发布 /temperature，订阅 /robot_command");
  }

private:
  void publishTemperature()
  {
    std_msgs::msg::Float64 msg;
    // 让温度在 22 ~ 28 之间来回波动（正弦式的粗糙模拟）
    msg.data = 25.0 + 3.0 * phase_;
    phase_ = -phase_;
    temp_pub_->publish(msg);
    RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 1000, "发布温度: %.2f", msg.data);
  }

  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr temp_pub_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr cmd_sub_;
  rclcpp::TimerBase::SharedPtr timer_;
  double phase_ = 1.0;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<RobotNode>());
  rclcpp::shutdown();
  return 0;
}
