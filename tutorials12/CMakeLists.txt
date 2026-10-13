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
project(bt_tutorial_12 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(behaviortree_cpp REQUIRED)

add_executable(bt_tutorial_12 main.cpp)

target_link_libraries(bt_tutorial_12 PRIVATE BT::behaviortree_cpp)
