#!/usr/bin/env bash
# ============================================================================
# run_tutorial.sh —— 运行本节教学示例（bt_ros2_tutorial）：
#   1) 启动 Countdown 动作服务端
#   2) 启动 AddTwoInts 服务端
#   3) 前台运行行为树执行器（跑完自动退出），退出时清理两个服务端
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

# 清理可能残留的旧实例（用 [x] 技巧避免匹配到本脚本自身）
pkill -f "[c]ountdown_server" 2>/dev/null || true
pkill -f "[a]dd_two_ints_server" 2>/dev/null || true
sleep 1

echo "[run] 启动 countdown_server ..."
ros2 run bt_ros2_tutorial countdown_server >"$ROOT/log/tutorial_countdown.log" 2>&1 &
P_COUNT=$!

echo "[run] 启动 add_two_ints_server ..."
ros2 run bt_ros2_tutorial add_two_ints_server >"$ROOT/log/tutorial_service.log" 2>&1 &
P_SVC=$!

cleanup() {
  kill "$P_COUNT" "$P_SVC" 2>/dev/null || true
  pkill -f "[c]ountdown_server" 2>/dev/null || true
  pkill -f "[a]dd_two_ints_server" 2>/dev/null || true
}
trap cleanup EXIT

# 等服务端就绪
sleep 2

echo "[run] 运行 tutorial_executor（行为树执行器）..."
echo "----------------------------------------------------------------"
ros2 run bt_ros2_tutorial tutorial_executor
echo "----------------------------------------------------------------"

echo
echo "[run] 两个服务端的日志（节选）："
echo "--- countdown_server ---"; sed -n '1,15p' "$ROOT/log/tutorial_countdown.log"
echo "--- add_two_ints_server ---"; sed -n '1,15p' "$ROOT/log/tutorial_service.log"
