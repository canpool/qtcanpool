# consume —— SDK 消费示例

演示下游工程如何通过 `find_package(QtCanpool)` 消费 qtcanpool 的安装产物（决策 B5/B6、
路线图验收标准第 6 条）。

它**不**引用源码树，只依赖一个安装前缀：包含公开头（`qxapp/qxappshell.h`）、链接导入目标
（`QtCanpool::qxapp`），其余一概不知。CI 会对它执行完整的「安装 → 消费」流程，因此 SDK 的
可消费性是被验证的，而不是被假设的。

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

## 消费 SDK

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="/path/to/sdk;<Qt 安装目录>"
cmake --build build --parallel
```

`CMAKE_PREFIX_PATH` 里**必须同时有 SDK 前缀与 Qt**：`QtCanpoolConfig.cmake` 会
`find_dependency(Qt5 ... COMPONENTS Core Gui Widgets)`。
