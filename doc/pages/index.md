# QtCanpool

**一套总结自 Qt Creator 源码结构的 Qt Widgets 项目管理模板与组件库。**

QtCanpool 提供一整套可直接用于产品开发的界面基础设施：Ribbon 功能区、可停靠窗口、无边框自定义窗口、
应用外壳、主题引擎，以及配置与日志这类应用层必备设施。它的目标是让一个 Qt 桌面项目**第一天就具备**
成熟产品的骨架 —— 统一的项目组织方式、一致的构建与测试流程、开箱可用的界面组件。

[![CI](https://github.com/canpool/qtcanpool/actions/workflows/ci.yml/badge.svg?branch=release-3.x)](https://github.com/canpool/qtcanpool/actions/workflows/ci.yml)
[![Docs](https://github.com/canpool/qtcanpool/actions/workflows/pages.yml/badge.svg?branch=release-3.x)](https://github.com/canpool/qtcanpool/actions/workflows/pages.yml)
[![Demo](https://img.shields.io/badge/Demo-online-2aa84a.svg)](https://canpool.github.io/qtcanpool/demo/)
[![License: MulanPSL-2.0](https://img.shields.io/badge/License-MulanPSL--2.0-blue.svg)](https://github.com/canpool/qtcanpool/blob/release-3.x/LICENSE)
[![Qt](https://img.shields.io/badge/Qt-5.15%20%7C%206.x-41CD52.svg)](https://www.qt.io/)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C.svg)](https://isocpp.org/)

## 为什么是 QtCanpool

| 关注点 | QtCanpool 的做法 |
| :--- | :--- |
| **项目组织** | 一套 CMake 框架管理多个项目：库、示例、插件、测试各有约定俗成的落点，新增模块只需一行 `add_subdirectory` |
| **构建与质量** | CMake 为主构建（qmake 自 3.0 起冻结）；[clang-format](https://github.com/canpool/qtcanpool/blob/release-3.x/.clang-format) 作为 CI 强制门禁；CTest 单元测试矩阵覆盖 Qt 5.15 与 Qt 6 |
| **界面组件** | Ribbon、Dock、无边框窗口三类现代桌面界面范式，均可在 Qt 5 与 Qt 6 上编译 |
| **应用骨架** | @ref QxApp::QxAppShell 把导航轨、页面栈、停靠区、状态栏与启动屏组装成一个可直接继承的窗口 |
| **主题** | @ref QxTheme::QxThemeManager 统一应用调色板与样式表，支持运行时切换、选择持久化与跟随系统深浅色 |
| **基础设施** | @ref QxCore::QxSettings 负责配置与版本迁移，@ref QxCore::QxLogger 负责带轮转的日志 |

## 组件总览

每个组件是一个独立的库、独立的命名空间，可以按需取用。完整说明见 [组件全览](components.md)。

| 组件 | 命名空间 | 说明 | 依赖 |
| :--- | :--- | :--- | :--- |
| `qxcore` | @ref QxCore | 基础设施：配置（`QxSettings`）、日志（`QxLogger`） | Qt Core |
| `qxtheme` | @ref QxTheme | 主题引擎：调色板 + 样式表统一应用、运行时切换、持久化、跟随系统 | `qxcore`、Qt Widgets |
| `qxwindow` | @ref QxWindow | 自定义窗口：无边框窗口、系统按钮代理、原生窗口上下文 | Qt Gui |
| `qxribbon` | @ref QxRibbon | Ribbon 风格界面：菜单栏 / 页 / 分组 / 快捷工具栏 | `qxwindow`、Qt Widgets |
| `qxdock` | @ref QxDock | 可停靠窗口：布局管理、标签化面板、浮动容器 | Qt Widgets |
| `qxapp` | @ref QxApp | 应用框架：`RibbonAppWindow`、`QxAppShell`、`QxNavigationBar`、`QxSplashScreen` | `qxribbon`、`qxdock`、`qxtheme`、`qxcore` |
| `qcanpool` | @ref QCanpool | 核心库，提供标题栏、工具按钮等通用控件（**legacy，已冻结**） | Qt Widgets |
| `qtcompat` | — | Qt 5 / Qt 6 跨版本兼容辅助头（header-only） | Qt Core |

> **注意**：`qcanpool` 中的 legacy Ribbon 系列（`ribbonbar`、`ribbonpage`、`ribbongroup`、`ribbonwindow`）
自 3.0 起冻结，新代码请使用 `qxribbon`。详见 [迁移指南](migration.md)。

## 快速开始

```bash
# 配置（通过 CMAKE_PREFIX_PATH 指定 Qt）
cmake -S . -B build -DCMAKE_PREFIX_PATH=<Qt安装目录>/<版本>/<编译器> -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build build --config Release --parallel

# 运行单元测试
ctest --test-dir build -C Release --output-on-failure
```

编译产物中的示例程序位于 `bin/`，例如 `RibbonDemo`、`DockDemo`、`AppShellDemo`。
完整的构建说明与开关列表见 [构建与安装](build.md)。

### 在线体验

`AppShellDemo` 已编译为 WebAssembly，可直接在浏览器中打开，无需安装 Qt 或编译器：

<https://canpool.github.io/qtcanpool/demo/>

它展示了一个完整的应用外壳：侧边导航、页面切换、可停靠面板、主题切换与布局持久化，全部运行在
`QxAppShell` 之上。

## 文档导览

| 页面 | 内容 |
| :--- | :--- |
| [构建与安装](build.md) | 环境要求、CMake 构建、开关、测试与文档站点构建 |
| [架构与约定](architecture.md) | 库分层、命名空间与 d-pointer 约定、CMake API、编码规范 |
| [组件全览](components.md) | 各组件的能力范围、主要类与最小用法 |
| [主题引擎](theming.md) | 主题引擎：内置主题、运行时切换、跟随系统、自定义主题 |
| [应用外壳](appshell.md) | 应用外壳：页面、停靠区、状态栏、布局持久化、启动屏 |
| [迁移指南](migration.md) | 2.x → 3.0 迁移：命名空间、类与方法对照、构建迁移 |

## 环境要求

| 项目 | 要求 |
| :--- | :--- |
| Qt | **5.15 及以上**（主推 Qt 6.5 / 6.8 LTS） |
| C++ | C++17 |
| CMake | 3.16 及以上（推荐使用最新版本） |
| 编译器 | 随 Qt 分发的 MinGW；MSVC 2019+；GCC 9+；Apple Clang |
| Doxygen（可选） | 1.9 及以上，仅构建文档站点时需要 |

## 仓库与许可

- GitHub：<https://github.com/canpool/qtcanpool>
- Gitee：<https://gitee.com/icanpool/qtcanpool>
- 使用教程：<https://blog.csdn.net/canpool/category_10631139.html>
- 许可：MulanPSL-2.0（集成组件遵循各自许可，见 `LICENSE.NOTES.md`）
