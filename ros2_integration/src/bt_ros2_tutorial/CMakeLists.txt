cmake_minimum_required(VERSION 3.16)
project(bt_ros2_tutorial)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# 找依赖
# ---------------------------------------------------------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(ament_index_cpp REQUIRED)

# 关键：这里的 behaviortree_cpp 不是 vcpkg 那个，而是 ROS 源的
# ros-humble-behaviortree-cpp（v4），它导出的目标名是 behaviortree_cpp::behaviortree_cpp
find_package(behaviortree_cpp REQUIRED)
# 官方封装库：提供 BT::RosActionNode / BT::RosServiceNode 等
find_package(behaviortree_ros2 REQUIRED)
# example_interfaces 提供 AddTwoInts 服务
find_package(example_interfaces REQUIRED)
# rosidl 生成器：用来把下面的 .action 编译成 C++ 类型
find_package(rosidl_default_generators REQUIRED)

# ---------------------------------------------------------------------------
# 自定义接口：Countdown.action -> bt_ros2_tutorial/action/countdown.hpp 等
# ---------------------------------------------------------------------------
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/Countdown.action"
)

# 行为树相关目标都要链接这两项
set(BT_LIBS
  behaviortree_cpp::behaviortree_cpp
  behaviortree_ros2::behaviortree_ros2
)

# ---------------------------------------------------------------------------
# 1) ROS 侧的“服务端”示例（普通 rclcpp 节点，跟行为树无关）
# ---------------------------------------------------------------------------
# 自定义 Action 的服务端：需要本包生成的 Countdown 类型
add_executable(countdown_server src/countdown_server.cpp)
target_link_libraries(countdown_server
  rclcpp::rclcpp
  rclcpp_action::rclcpp_action
  ${PROJECT_NAME}__rosidl_typesupport_cpp)

# AddTwoInts 服务端：需要 example_interfaces 的类型
add_executable(add_two_ints_server src/add_two_ints_server.cpp)
target_link_libraries(add_two_ints_server
  rclcpp::rclcpp
  ${example_interfaces_TARGETS})

# ---------------------------------------------------------------------------
# 2) 行为树这一侧：执行器（里面注册并使用上面两个 ROS 节点）
# ---------------------------------------------------------------------------
add_executable(tutorial_executor src/tutorial_executor.cpp)
target_include_directories(tutorial_executor PRIVATE include)
target_link_libraries(tutorial_executor
  ${BT_LIBS}
  rclcpp::rclcpp
  ament_index_cpp::ament_index_cpp
  ${PROJECT_NAME}__rosidl_typesupport_cpp
  ${example_interfaces_TARGETS})

# ---------------------------------------------------------------------------
# 安装
# ---------------------------------------------------------------------------
install(TARGETS countdown_server add_two_ints_server tutorial_executor
  DESTINATION lib/${PROJECT_NAME})

# XML / launch 装到 share，运行期可通过 ament 索引定位
install(DIRECTORY behavior_trees DESTINATION share/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

ament_export_dependencies(rosidl_default_runtime)
ament_package()
