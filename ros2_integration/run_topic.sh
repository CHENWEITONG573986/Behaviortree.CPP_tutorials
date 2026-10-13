#!/usr/bin/env bash
# ============================================================================
# run_topic.sh —— 运行 Topic 教学示例：
#   1) 启动“模拟机器人”节点（发布 /temperature，订阅 /robot_command）
#   2) 前台运行 Topic 版行为树执行器，退出时清理模拟机器人
# ============================================================================
set -eo pipefail
source "$(cd "$(dirname "$0")" && pwd)/env.sh"

if [ ! -f "$ROOT/install/setup.bash" ]; then
  echo "还没构建，先执行 ./build.sh" >&2
  exit 1
fi
# shellcheck disable=SC1091
source "$ROOT/install/setup.bash"

mkdir -p "$ROOT/log"

# 清理可能残留的旧实例
pkill -f "[r]obot_node" 2>/dev/null || true
sleep 1

echo "[run] 启动 robot_node（发布 /temperature，订阅 /robot_command）..."
ros2 run bt_ros2_tutorial robot_node >"$ROOT/log/tutorial_robot.log" 2>&1 &
P_ROBOT=$!

cleanup() {
  kill "$P_ROBOT" 2>/dev/null || true
  pkill -f "[r]obot_node" 2>/dev/null || true
}
trap cleanup EXIT

sleep 2

echo "[run] 运行 topic_executor ..."
echo "----------------------------------------------------------------"
ros2 run bt_ros2_tutorial topic_executor
echo "----------------------------------------------------------------"

echo
echo "[run] 模拟机器人日志（节选，注意最后收到的指令）："
sed -n '1,20p' "$ROOT/log/tutorial_robot.log"
