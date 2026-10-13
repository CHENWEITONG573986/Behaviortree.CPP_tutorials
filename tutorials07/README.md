# 教程 07：使用多个 XML 文件

官方文档：<https://www.behaviortree.dev/docs/tutorial-basics/tutorial_07_multiple_xml>

## 本节要点

子树多了以后，把每棵树拆到独立的 XML 文件里更清晰。有两种加载方式：

1. **手动加载（推荐）**：遍历目录，把所有 `.xml` 都 `registerBehaviorTreeFromFile()`。
2. **使用 `<include>`**：只在 `main_tree.xml` 里写

   ```xml
   <root BTCPP_format="4">
       <include path="./subtree_A.xml" />
       <include path="./subtree_B.xml" />
       <BehaviorTree ID="MainTree"> ... </BehaviorTree>
   </root>
   ```

   路径相对于 `main_tree.xml` 所在位置。

本目录的文件：

- `main_tree.xml` —— 主树，引用 `SubTreeA` / `SubTreeB`
- `subtree_A.xml`、`subtree_B.xml`

## 编译与运行

```bash
cd tutorials07
cmake -S . -B build
cmake --build build
./build/bt_tutorial_07        # 需在本目录运行（会扫描 ./ 下的 .xml）
```

## 期望输出

```
Registered BehaviorTrees:
 - MainTree
 - SubTreeA
 - SubTreeB
----- MainTree tick ----
Robot says: starting MainTree
Robot says: Executing Sub_A
Robot says: Executing Sub_B
----- SubTreeA tick ----
Robot says: Executing Sub_A
```
