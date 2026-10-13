#!/usr/bin/env bash
# ============================================================================
# setup.sh —— 一步准备好 ROS 2 集成所需的东西：
#   1) BT.CPP v4（优先用 apt 的 ros-humble-behaviortree-cpp，没有则下载 deb
#      解包到本地 prefix/，不需要 sudo）
#   2) 克隆官方 BehaviorTree.ROS2（固定到已验证的 humble 提交）
# ============================================================================
set -eo pipefail
source "$(cd "$(dirname "$0")" && pwd)/env.sh"

# ---- 1) BT.CPP v4 -----------------------------------------------------------
if [ "$BT_PREFIX" = "/opt/ros/humble" ]; then
  echo "[setup] 检测到系统已安装 ros-humble-behaviortree-cpp，直接使用。"
else
  echo "[setup] 系统没有 ros-humble-behaviortree-cpp。"
  echo "        推荐（更干净）：  sudo apt install ros-humble-behaviortree-cpp"
  echo "        现在改为下载 deb 解包到 $ROOT/prefix（无需 sudo）..."
  mkdir -p "$ROOT/prefix"
  tmpdeb="$(mktemp -d)"
  ( cd "$tmpdeb" && apt-get download ros-humble-behaviortree-cpp >/dev/null )
  rm -rf "${ROOT:?}/prefix/opt"
  dpkg-deb -x "$tmpdeb"/ros-humble-behaviortree-cpp_*.deb "$ROOT/prefix"
  rm -rf "$tmpdeb"
  echo "[setup] 已解包到 $BT_PREFIX"
fi

# ---- 2) BehaviorTree.ROS2 源码 ---------------------------------------------
mkdir -p "$ROOT/src"
if [ ! -d "$BTROS2_DIR/.git" ]; then
  echo "[setup] 克隆 $BTROS2_REPO ..."
  git clone "$BTROS2_REPO" "$BTROS2_DIR"
fi
git -C "$BTROS2_DIR" fetch -q origin
git -C "$BTROS2_DIR" checkout -q "$BTROS2_REF"
echo "[setup] BehaviorTree.ROS2 -> $BTROS2_REF"

echo
echo "[setup] 完成。下一步：  ./build.sh"
