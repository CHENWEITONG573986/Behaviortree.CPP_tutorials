#!/usr/bin/env bash
# ============================================================================
# env.sh —— 被 setup.sh / build.sh / run_demo.sh 共同 source 的环境准备脚本。
# ============================================================================
export ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# ---- ROS 2 Humble -----------------------------------------------------------
if [ -z "${ROS_DISTRO:-}" ]; then
  # shellcheck disable=SC1091
  source /opt/ros/humble/setup.bash
fi

# ---- BT.CPP v4 (>= 4.6) -----------------------------------------------------
# behaviortree_ros2 只兼容 BT.CPP 4.6+，并且要求 CMake 目标名
# `behaviortree_cpp::behaviortree_cpp`（vcpkg 版导出的是 BT::behaviortree_cpp，
# 不匹配；ROS 源的 ros-humble-behaviortree-cpp 才匹配）。
#
# 优先使用系统安装的 ros-humble-behaviortree-cpp；
# 若没有，则用 setup.sh 解包到本目录 prefix/ 的本地副本（无需 sudo）。
if [ -d /opt/ros/humble/share/behaviortree_cpp ]; then
  export BT_PREFIX=/opt/ros/humble
else
  export BT_PREFIX="$ROOT/prefix/opt/ros/humble"
fi

export CMAKE_PREFIX_PATH="$BT_PREFIX:${CMAKE_PREFIX_PATH:-}"
export AMENT_PREFIX_PATH="$BT_PREFIX:${AMENT_PREFIX_PATH:-}"
export LD_LIBRARY_PATH="$BT_PREFIX/lib:${LD_LIBRARY_PATH:-}"

# ---- 官方 BehaviorTree.ROS2 仓库（固定到已验证的 humble 提交）--------------
export BTROS2_REPO="${BTROS2_REPO:-https://github.com/BehaviorTree/BehaviorTree.ROS2}"
export BTROS2_REF="${BTROS2_REF:-6c6aa078ee7bc52fec98984bed4964556abf5beb}"
export BTROS2_DIR="$ROOT/src/BehaviorTree.ROS2"
