# 构建与安装

本页说明如何从源码构建 QtCanpool、如何选择组件、如何运行测试以及如何生成文档站点。

## 环境要求

| 项目 | 要求 |
| :--- | :--- |
| Qt | **5.15 及以上**，主推 Qt 6.5 / 6.8 LTS |
| C++ | C++17（构建框架强制 `CMAKE_CXX_STANDARD 17`，且不允许编译器扩展） |
| CMake | 3.16 及以上 |
| 编译器 | MinGW（随 Qt 分发）、MSVC 2019+、GCC 9+、Apple Clang |
| Doxygen（可选） | 1.9 及以上，仅在构建文档站点时需要 |
| Graphviz（可选） | 提供 `dot` 后类继承图才会生成 |

## 快速开始

```bash
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=<Qt安装目录>/<版本>/<编译器> \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release --parallel
```

`CMAKE_PREFIX_PATH` 是选用 Qt 的唯一入口，例如 `C:/Qt/6.8.3/mingw_64`。
也可以在 Qt Creator 中直接打开根目录的 `CMakeLists.txt`。

## 功能开关

开关均为 CMake 缓存变量，可在配置时通过 `-D<名字>=ON|OFF` 指定，
配置完成后用 `cmake -S . -B build -LH` 可以列出全部开关及当前取值。

| 开关 | 默认 | 说明 |
| :--- | :--- | :--- |
| `WITH_DEMOS` | `ON` | 构建 `demos/` 下的综合示例 |
| `WITH_TESTS` | `OFF` | 构建 `tests/` 下的单元测试并注册到 CTest |
| `WITH_DOCS` | `OFF` | 生成 Doxygen 文档站点（需要 Doxygen） |
| `WITH_ONLINE_DOCS` | `OFF` | 生成在线文档（qdoc 通道，遗留开关） |
| `BUILD_WITH_PCH` | `ON` | 使用预编译头加速构建 |
| `WITH_SANITIZE` | `OFF` | 打开地址/未定义行为检测器 |
| `BUILD_LINK_WITH_QT` | `OFF` | 作为 Qt Creator 的子项目构建时链接其 Qt |
| `SHOW_BUILD_DATE` | `OFF` | 在关于对话框中显示构建日期 |
| `ENABLE_SVG_SUPPORT` | 自动 | 检测到 Qt SVG 时自动打开 |

### 组件开关

每个库都可以单独关闭，关闭后依赖它的上层库会一并跳过，因此可以只构建需要的部分：

| 开关 | 默认 | 说明 |
| :--- | :--- | :--- |
| `WITH_QXCORE` | `ON` | 基础设施库（配置、日志） |
| `WITH_QXTHEME` | `ON` | 主题引擎（需要 `WITH_QXCORE`） |
| `WITH_QCANPOOL` | `ON` | legacy 核心库（已冻结） |
| `WITH_QXRIBBON` | `ON` | Ribbon 界面组件 |
| `WITH_QXDOCK` | `ON` | 可停靠窗口组件 |
| `WITH_QXWINDOW` | `ON` | 无边框自定义窗口组件 |
| `WITH_QXAPP` | `ON` | 应用框架（需要上面全部） |

## Qt 5 与 Qt 6

同一份源码同时支持两个大版本。构建框架中的 `cmake/FindQt5.cmake` 是一个 **Qt 6 优先的包装层**：
它把 `Qt5::X` 目标别名到 `Qt6::X`，因此库内部始终写 `Qt5::Core`、`Qt5::Widgets` 这类依赖，
实际链接到哪个版本取决于 `CMAKE_PREFIX_PATH`。

需要注意的少数差异：

- 高 DPI：Qt 6 默认开启，Qt 5 必须在构造 `QApplication` **之前**设置属性。
  `demos/qxapp/appshell/main.cpp` 给出了一份可直接复用的模板。
- `QAction` / `QActionGroup`：Qt 6 位于 `QtGui`，Qt 5 位于 `QtWidgets`。
  项目约定使用**不带模块前缀**的 `#include <QAction>`，两个版本都能解析。
- `Q_DECLARE_METATYPE`：Qt 5.15 下需要显式包含 `<QtCore/QMetaType>`。

## 运行测试

```bash
ctest --test-dir build -C Release --output-on-failure
```

测试通过 `add_qtc_test()` 注册，它为每个测试处理好 rpath、AUTOMOC 与头文件搜索路径
（公共包含目录是 `src/libs`，所以测试可以直接 `#include "qxribbon/xxx.h"`）。

在**没有交互桌面**的环境（CI、远程会话）中运行界面相关测试时，需要指定离屏平台：

```bash
QT_QPA_PLATFORM=offscreen ctest --test-dir build -C Release --output-on-failure
```

单个测试可以只跑一个用例类：

```bash
QTC_ONLY_TEST=tst_QxAppShell ./bin/tst_qxapp
```

## 构建文档站点

`doc/` 目录本身是一个独立的 CMake 工程，只依赖 Doxygen 与可选的 Graphviz，**不需要 Qt**：

```bash
cmake -S doc -B build-docs
cmake --build build-docs --target docs
# 产物：build-docs/html/index.html
```

也可以作为主构建的一部分生成：

```bash
cmake -S . -B build -DWITH_DOCS=ON -DCMAKE_PREFIX_PATH=<Qt>
cmake --build build --target docs
```

可用的文档相关缓存变量：

| 变量 | 说明 |
| :--- | :--- |
| `DOXYGEN_EXECUTABLE` | 指定要使用的 Doxygen 可执行文件 |
| `DOXYGEN_PROJECT_NUMBER` | 覆盖页面中显示的版本号（默认取 `cmake/QtCanpoolBranding.cmake`） |
| `DOXYGEN_OUTPUT_LANGUAGE` | 站点界面语言，默认 `Chinese` |
| `DOXYGEN_WARN_AS_ERROR` | 设为 `YES` 或 `FAIL_ON_WARNINGS` 可让告警直接导致构建失败 |

## 示例程序

构建完成后，可执行文件位于构建目录的 `bin/`：

| 示例 | 目标 | 说明 |
| :--- | :--- | :--- |
| `fancydemo` | `FancyDemo` | 综合示例：标题栏、工具栏、侧边栏等控件 |
| `ribbondemo` | `RibbonDemo` | Ribbon 界面：页、分组、快捷工具栏、应用按钮 |
| `dockdemo` | `DockDemo` | 可停靠窗口：中央区域、浮动、标签化、布局保存 |
| `appshell` | `AppShellDemo` | 应用外壳：导航轨 + 页面栈 + 停靠区 + 主题 + 启动屏 + 布局持久化 |

`examples/` 下另有更细粒度的控件示例（qmake 工程）。

## 安装

```bash
cmake --install build --prefix <安装目录>
```

头文件安装到 `<prefix>/include`，库安装到 `<prefix>/lib`，并导出 CMake 包配置，
下游工程可以 `find_package(QtCanpool)` 后直接链接 `QtCanpool::qxcore` 这样的目标。

## 关于 qmake

根目录仍保留 `qtcanpool.pro` 等 qmake 工程文件，但**自 3.0 起已冻结为只读**：
新增的库（`qxcore`、`qxtheme`）与 `qxapp` 的新增源码都**只进 CMake**，
qmake 侧继续编译它一贯的源码列表。qmake 工程计划在后续版本中移除。

> **注意**：如果需要在 qmake 构建中使用 3.0 新增的库，请先确认这一决策是否需要调整。
