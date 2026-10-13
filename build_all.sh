#!/usr/bin/env bash
# ============================================================================
# 一键编译（并可运行）所有教程。
#
# 用法：
#   ./build_all.sh          # 编译并运行 tutorials01 .. tutorials16
#   ./build_all.sh 05       # 只处理 tutorials05
#   ./build_all.sh 05 06    # 处理多个
#
# 说明：vcpkg 的配置写在每个 tutorialsNN/CMakeLists.txt 里，
#       所以这里不需要再传 -DCMAKE_TOOLCHAIN_FILE。
# ============================================================================
set -u

ROOT="$(cd "$(dirname "$0")" && pwd)"

# 收集要处理的目录
if [ "$#" -gt 0 ]; then
  DIRS=()
  for n in "$@"; do
    DIRS+=("$ROOT/tutorials$(printf '%02d' "$((10#$n))")")
  done
else
  DIRS=("$ROOT"/tutorials*)
fi

for dir in "${DIRS[@]}"; do
  [ -d "$dir" ] || { echo "跳过（不存在）: $dir"; continue; }
  name="$(basename "$dir")"
  num="${name#tutorials}"
  exe="$dir/build/bt_tutorial_${num}"

  echo "============================== $name =============================="
  if ! ( cd "$dir" && cmake -S . -B build >/dev/null ); then
    echo "!! $name 配置失败"
    continue
  fi
  if ! cmake --build "$dir/build"; then
    echo "!! $name 编译失败"
    continue
  fi

  if [ -x "$exe" ]; then
    echo "------------------------------ 运行 $name ------------------------------"
    ( cd "$dir" && "./build/bt_tutorial_${num}" )
  else
    echo "（$name 没有生成可执行文件，通常是因为该教程依赖的库功能未启用，见其 README）"
  fi
  echo
done

echo "全部完成。"
