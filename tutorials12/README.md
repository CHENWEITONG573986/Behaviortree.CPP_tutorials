# 教程 12：端口的默认值

官方文档：<https://www.behaviortree.dev/docs/tutorial-advanced/tutorial_12_default_ports>

## 本节要点

定义端口时可以给一个“默认值”，当 XML 没有为该端口赋值时使用。写法有 5 种：

| 端口 | 默认值来源 |
|------|------------|
| `pointA` | 直接给值 `Point2D{1,2}` |
| `pointB` | 指向黑板键 `{point}` |
| `pointC` | 字符串 `"5,6"`，运行时转换成 `Point2D` |
| `pointD` | `{=}` —— 默认指向**同名**黑板键 `pointD` |
| `pointE` | JSON 形式 `json:{"x":9,"y":10}` |

> 注意 `pointB` / `pointD` 的默认值来自黑板，所以 main() 里先 `blackboard->set(...)`
> 把 `point` 和 `pointD` 准备好。

## 编译与运行

```bash
cd tutorials12
cmake -S . -B build
cmake --build build
./build/bt_tutorial_12
```

## 期望输出

```
Result: SUCCESS
```
