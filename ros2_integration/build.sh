#!/usr/bin/env bash
# ============================================================================
# build.sh —— 用 colcon 构建工作区（behaviortree_ros2 + 接口 + 官方 samples）。
# ============================================================================
set -eo pipefail
source "$(cd "$(dirname "$0")" && pwd)/env.sh"

cd "$ROOT"
# 只在 src/ 下寻找 ROS 包：否则 colcon 会把 prefix/（BT.CPP 的本地副本，
# 里面也有 package.xml）误当成待编译的包。
colcon build --base-paths "$ROOT/src" "$@"

echo
echo "[build] 完成。下一步：  ./run_demo.sh   （或 ./run_demo.sh SleepActionSample）"
