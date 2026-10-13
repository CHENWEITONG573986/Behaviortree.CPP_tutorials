// ============================================================================
// sample_nodes/dummy_nodes.cpp
// ----------------------------------------------------------------------------
// dummy_nodes.h 里声明的节点实现。
// 顶部那个 BT_REGISTER_NODES 宏用于把节点编译成一个可以被运行时加载的
// 插件（本教程都是静态链接，这个入口只是顺带保留）。
// ============================================================================
#include "dummy_nodes.h"

// 若把本文件编译成共享库，这个宏会生成运行时注册（插件）入口。
// NOLINTNEXTLINE(misc-use-anonymous-namespace)
BT_REGISTER_NODES(factory)
{
  DummyNodes::RegisterNodes(factory);
}

namespace DummyNodes
{

BT::NodeStatus CheckBattery()
{
  std::cout << "[ Battery: OK ]" << std::endl;
  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus CheckTemperature()
{
  std::cout << "[ Temperature: OK ]" << std::endl;
  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus SayHello()
{
  std::cout << "Robot says: Hello World" << std::endl;
  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus GripperInterface::open()
{
  _opened = true;
  std::cout << "GripperInterface::open" << std::endl;
  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus GripperInterface::close()
{
  std::cout << "GripperInterface::close" << std::endl;
  _opened = false;
  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus ApproachObject::tick()
{
  std::cout << "ApproachObject: " << this->name() << std::endl;
  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus SaySomething::tick()
{
  auto msg = getInput<std::string>("message");
  if(!msg)
  {
    throw BT::RuntimeError("missing required input [message]: ", msg.error());
  }

  std::cout << "Robot says: " << msg.value() << std::endl;
  return BT::NodeStatus::SUCCESS;
}

BT::NodeStatus SaySomethingSimple(BT::TreeNode& self)
{
  auto msg = self.getInput<std::string>("message");
  if(!msg)
  {
    throw BT::RuntimeError("missing required input [message]: ", msg.error());
  }

  std::cout << "Robot says: " << msg.value() << std::endl;
  return BT::NodeStatus::SUCCESS;
}

}  // namespace DummyNodes
