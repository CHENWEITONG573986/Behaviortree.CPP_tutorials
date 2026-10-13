cmake_minimum_required(VERSION 3.16)

# ---------------------------------------------------------------------------
# 自动使用 vcpkg，省去命令行参数 -DCMAKE_TOOLCHAIN_FILE=...
# 优先使用环境变量 VCPKG_ROOT，没有则回退到本机默认路径。
# 注意：这段必须放在 project() 之前。
# ---------------------------------------------------------------------------
if(NOT CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VCPKG_ROOT})
    set(_vcpkg_root "$ENV{VCPKG_ROOT}")
  else()
    set(_vcpkg_root "/home/rpp/vcpkg")
  endif()
  set(CMAKE_TOOLCHAIN_FILE "${_vcpkg_root}/scripts/buildsystems/vcpkg.cmake"
      CACHE FILEPATH "vcpkg toolchain file")
endif()
project(bt_tutorial_11 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(behaviortree_cpp REQUIRED)

# Groot2Publisher 只有在 behaviortree_cpp 启用了 Groot2 接口时才存在。
# 这里做一个编译+链接探测：能链接到 Groot2Publisher 的成员函数才算可用。
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_LIBRARIES BT::behaviortree_cpp)
check_cxx_source_compiles("
#include <behaviortree_cpp/loggers/groot2_publisher.h>
int main() {
  auto f = &BT::Groot2Publisher::setMaxHeartbeatDelay;
  (void)f;
  return 0;
}
" BT_HAS_GROOT2_PUBLISHER)

if(NOT BT_HAS_GROOT2_PUBLISHER)
  message(WARNING
    "当前 behaviortree_cpp 未启用 Groot2 接口 (BTCPP_GROOT_INTERFACE=OFF)，"
    "教程 11 的可执行文件被跳过。启用方法见 tutorials11/README.md。")
  return()
endif()

set(SAMPLE_NODES ${CMAKE_CURRENT_SOURCE_DIR}/../sample_nodes)

add_executable(bt_tutorial_11
  main.cpp
  ${SAMPLE_NODES}/crossdoor_nodes.cpp)

target_include_directories(bt_tutorial_11 PRIVATE ${SAMPLE_NODES})

target_link_libraries(bt_tutorial_11 PRIVATE BT::behaviortree_cpp)
