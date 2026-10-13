// ============================================================================
// countdown_server.cpp
// ----------------------------------------------------------------------------
// Countdown 这个 ROS 2 Action 的“服务端”（普通 rclcpp 节点，跟行为树无关）。
//
// 它做的事：收到 {count} 后，从 count 开始每秒减 1，每次都把当前值作为
// feedback 发出去；减到 0 时返回 result（一句完成说明）。
//
// 一个 Action 服务端要实现三个回调：
//   handle_goal()     —— 收到目标：决定接受还是拒绝
//   handle_cancel()   —— 收到取消请求：决定是否同意取消
//   handle_accepted() —— 目标被接受后：真正开始执行（通常放到单独线程里，
//                        因为不能阻塞 executor）
// ============================================================================
#include <chrono>
#include <memory>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

#include "bt_ros2_tutorial/action/countdown.hpp"

using Countdown = bt_ros2_tutorial::action::Countdown;
using GoalHandle = rclcpp_action::ServerGoalHandle<Countdown>;

class CountdownServer : public rclcpp::Node
{
public:
  CountdownServer() : Node("countdown_server")
  {
    using namespace std::placeholders;

    // 创建 Action 服务端，服务名固定为 "countdown"
    // （要和执行器里 RosNodeParams::default_port_value 一致）
    server_ = rclcpp_action::create_server<Countdown>(
        this, "countdown",
        std::bind(&CountdownServer::handle_goal, this, _1, _2),
        std::bind(&CountdownServer::handle_cancel, this, _1),
        std::bind(&CountdownServer::handle_accepted, this, _1));

    RCLCPP_INFO(get_logger(), "Countdown 动作服务端已启动，服务名: countdown");
  }

private:
  rclcpp_action::Server<Countdown>::SharedPtr server_;

  // 收到目标时调用：这里简单接受所有目标
  rclcpp_action::GoalResponse
  handle_goal(const rclcpp_action::GoalUUID&, std::shared_ptr<const Countdown::Goal> goal)
  {
    RCLCPP_INFO(get_logger(), "收到目标: count=%d", goal->count);
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  // 收到取消请求时调用
  rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandle> goal_handle)
  {
    RCLCPP_INFO(get_logger(), "收到取消请求");
    (void)goal_handle;
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  // 目标被接受后调用。这里必须尽快返回，所以把真正的执行放到一个线程里。
  void handle_accepted(const std::shared_ptr<GoalHandle> goal_handle)
  {
    std::thread{ std::bind(&CountdownServer::execute, this, std::placeholders::_1),
                 goal_handle }
        .detach();
  }

  // 真正的执行逻辑
  void execute(const std::shared_ptr<GoalHandle> goal_handle)
  {
    const auto goal = goal_handle->get_goal();
    auto feedback = std::make_shared<Countdown::Feedback>();
    auto result = std::make_shared<Countdown::Result>();

    for(int i = goal->count; i > 0; --i)
    {
      // 如果客户端取消了目标，就中止并回报“已取消”
      if(goal_handle->is_canceling())
      {
        result->message = "被取消";
        goal_handle->canceled(result);
        RCLCPP_INFO(get_logger(), "目标被取消");
        return;
      }

      feedback->current = i;
      goal_handle->publish_feedback(feedback);  // 发进度
      RCLCPP_INFO(get_logger(), "倒计时: %d", i);

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    result->message = "倒计时完成";
    goal_handle->succeed(result);  // 回报成功
    RCLCPP_INFO(get_logger(), "目标完成");
  }
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CountdownServer>());
  rclcpp::shutdown();
  return 0;
}
