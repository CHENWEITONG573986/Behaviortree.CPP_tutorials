# 教程 14：子树模型（Subtree Models）与自动重映射

官方文档：<https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_14_subtree_model>

## 本节要点

- 可以在 `<TreeNodesModel>` 里用 `<SubTree ID="...">` 给子树声明**端口模型**：
  有哪些输入/输出端口、有没有默认值/默认重映射。
- 有 `default` 的端口，调用方可以不写；没有 `default` 的端口，调用方**必须**提供。
- 这样子树端口变得“自描述”，Groot2 也能据此给出提示。

## 本例的端口模型（`MySub`）

| 端口 | 方向 | 默认值/重映射 |
|------|------|----------------|
| `sub_in_value`  | 输入 | `42`（默认值） |
| `sub_in_name`   | 输入 | 无（必填） |
| `sub_out_result`| 输出 | 默认重映射到 `{out_result}` |
| `sub_out_state` | 输出 | 无（必填） |

`MainTree` 只显式写了 `sub_in_name` 和 `sub_out_state`，其余走默认。

## 编译与运行

```bash
cd tutorials14
cmake -S . -B build
cmake --build build
./build/bt_tutorial_14
```

程序挂了一个 `StdCoutLogger`，会打印节点的状态转移；整个 Sequence 应当成功结束。
