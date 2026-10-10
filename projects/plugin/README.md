# plugin —— 外部插件验收工程

验证路线图 DoD 第 1 条：**一个只依赖 `QtCanpool::qxplugin` 的外部 CMake 工程，能产出可用插件**。

它不引用源码树，只依赖一个安装前缀——不 `include` 任何内部头，不调 `add_qtc_plugin()`，
`CMAKE_MODULE_PATH` 里也没有本仓库的 `cmake/`。CI 会拿它跑完整的「安装 → 消费」流程，
所以「插件可独立编译」是被验证的，而不是被假设的。

## 两个目标，两个问题

| 目标 | 回答的问题 |
| :--- | :--- |
| `acceptance`（MODULE 库） | 外部工程**能不能造出**插件？（编译 + 链接 + 元数据嵌入） |
| `qxpluginhost`（可执行） | 造出来的插件**能不能被认出来**？（发现 + 转型 + `initialize()`） |

第二个目标不能省。C2 时期就踩过一次只靠编译证明不了的坑：`QxPlugin` 曾被声明成 Qt 接口
（`Q_DECLARE_INTERFACE`），结果**外挂插件加载成功却永远 `qobject_cast` 失败**——`QPluginLoader`
一路 `load true` / `inst true`，管理器却报 "could not be instantiated"。只有把插件真正跑一遍才发现。

`qxpluginhost` 顺便是「宿主最少要写什么」的参考答案：一个 `QxPluginContext` 的实现 + 六行装配。
它这里每个 host 能力都是空实现，真实宿主把它们填上即可。

## 构建 SDK

```bash
# 1) 配置并编译本仓库
cmake -S ../.. -B ../../build -DCMAKE_BUILD_TYPE=Release
cmake --build ../../build --config Release --parallel

# 2) 安装到一个前缀（库 + 头文件 + CMake 包）
cmake --install ../../build --config Release --prefix /path/to/sdk
cmake --install ../../build --config Release --prefix /path/to/sdk --component Devel
```

> 头文件与 `QtCanpoolConfig.cmake` 属于 `Devel` 组件（`EXCLUDE_FROM_ALL`），所以需要**显式**
> 安装该组件，否则只有可执行文件与运行库落到前缀里。

## 构建并运行验收

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="/path/to/sdk;<Qt 安装目录>"
cmake --build build --parallel

# 无界面环境下要给 Qt 一个平台插件
QT_QPA_PLATFORM=offscreen ./build/qxpluginhost
```

期望输出（退出码 0）：

```
plugin dir: <build>/plugins
found:   yes
started: yes
```

`found` 为 `no` → 插件的 `Q_PLUGIN_METADATA` 没被 moc 带走（IID 写错、`FILE` 指错），
或产物没落在 `plugins/` 里；`started` 为 `no` → 元数据里的 `Name` 缺失，或 `initialize()`
返回了 false（`error:` 一行会说明原因）。两者都会让进程退出码非 0。

> `CMAKE_PREFIX_PATH` 里**必须同时有 SDK 前缀与 Qt**：`QtCanpoolConfig.cmake` 会
> `find_dependency(Qt5 ... COMPONENTS Core Gui Widgets)`。
