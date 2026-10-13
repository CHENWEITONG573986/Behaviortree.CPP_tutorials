#!/usr/bin/env bash
# ============================================================================
# run_demo.sh —— 跑官方示例：
#   1) 启动 sample_bt_executor（一个以 BT.CPP 为核心、用 ROS Action 对外服务的
#      “任务规划器”）
#   2) 用 ros2 action send_goal 触发一棵行为树
#
# 用法：
#   ./run_demo.sh                  # 默认执行 CrossDoor
#   ./run_demo.sh SleepActionSample
# ============================================================================
set -eo pipefail
source "$(cd "$(dirname "$0")" && pwd)/env.sh"

if [ ! -f "$ROOT/install/setup.bash" ]; then
  echo "还没构建，先执行 ./build.sh" >&2
  exit 1
fi
# shellcheck disable=SC1091
source "$ROOT/install/setup.bash"

TREE="${1:-CrossDoor}"
mkdir -p "$ROOT/log"
LOG="$ROOT/log/launch.log"

# 先清掉可能残留的旧实例，避免 1667（Groot2）等端口冲突
# 注意：Linux 进程名(comm)最多 15 字符，"sample_bt_executor" 会被截断，
# 所以这里用 -f 匹配完整命令行，并用 [s] 技巧避免匹配到 pkill 自身。
pkill -f "[s]ample_bt_executor" 2>/dev/null || true
pkill -f "[s]leep_server" 2>/dev/null || true
sleep 1

echo "[run] 启动 sample_bt_executor ...（日志：$LOG）"
ros2 launch btcpp_ros2_samples sample_bt_executor.launch.xml >"$LOG" 2>&1 &
LAUNCH_PID=$!
cleanup() {
  kill "$LAUNCH_PID" 2>/dev/null || true
  pkill -f "[s]ample_bt_executor" 2>/dev/null || true
  pkill -f "[s]leep_server" 2>/dev/null || true
}
trap cleanup EXIT

# 等 action server 就绪
for _ in $(seq 1 40); do
  if ros2 action list 2>/dev/null | grep -q behavior_server; then break; fi
  sleep 0.5
done

echo "[run] 发送目标： $TREE"
ros2 action send_goal /behavior_server btcpp_ros2_interfaces/action/ExecuteTree \
  "{target_tree: $TREE}"

echo
echo "[run] 执行器日志（节选）："
sed -n '1,25p' "$LOG"
