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
| **布局与工作区** | `saveLayout()` 记下窗口几何与停靠布局；@ref QxApp::QxWorkspaceManager 在此之上给排列起名字，可存多套并随时切换（3.3） |
| **主题** | @ref QxTheme::QxThemeManager 统一应用调色板与样式表，支持运行时切换、选择持久化与跟随系统深浅色 |
| **基础设施** | @ref QxCore::QxSettings 负责配置与版本迁移，@ref QxCore::QxLogger 负责带轮转的日志，@ref QxCore::QxTranslator 负责语言切换 |
| **开箱即用** | 生成的工程（见 `scripts/new-project`）已接好切语言、@ref QxApp::QxToast 应用内通知与 @ref QxApp::QxSettingsDialog 设置界面 |

## 组件总览

每个组件是一个独立的库、独立的命名空间，可以按需取用。完整说明见 [组件全览](components.md)。

| 组件 | 命名空间 | 说明 | 依赖 |
| :--- | :--- | :--- | :--- |
| `qxcore` | @ref QxCore | 基础设施：配置（`QxSettings`）、日志（`QxLogger`）、语言切换（`QxTranslator`） | Qt Core |
| `qxtheme` | @ref QxTheme | 主题引擎：调色板 + 样式表统一应用、运行时切换、持久化、跟随系统 | `qxcore`、Qt Widgets |
| `qxwindow` | @ref QxWindow | 自定义窗口：无边框窗口、系统按钮代理、原生窗口上下文 | Qt Gui |
| `qxribbon` | @ref QxRibbon | Ribbon 风格界面：菜单栏 / 页 / 分组 / 快捷工具栏 | `qxwindow`、Qt Widgets |
| `qxdock` | @ref QxDock | 可停靠窗口：布局管理、标签化面板、浮动容器 | Qt Widgets |
| `qxapp` | @ref QxApp | 应用框架：`RibbonAppWindow`、`QxAppShell`、`QxNavigationBar`、`QxSplashScreen`、应用内通知（`QxToast`）、设置界面（`QxPropertyEditor` / `QxSettingsDialog`） | `qxribbon`、`qxdock`、`qxtheme`、`qxcore` |
| `qtcompat` | — | Qt 5 / Qt 6 跨版本兼容辅助头（header-only） | Qt Core |

> **注意**：`qcanpool` 已于 **3.2 删除**。它在 3.1 已被清空（legacy Ribbon 系列物理移除、
> 11 个遗留控件下线、7 个通用控件迁入 `qxapp` 并改名），只剩 7 个带弃用告警的转发头；
> 3.2 连这个库一起消失，**旧名不再存在**。详见 [迁移指南](migration.md)。

## 版本对照

**项目版本**（tag 打在它上面）是发布列车号；**每个库另有自己的版本号**，两者互不绑定（K11）。
每次项目发版**结算一次**：这一批里**动了 API 的库才前进号**，没动的库保持原号。所以 3.0 → 3.3
项目版本走了四位，而 `qxtheme` / `qxribbon` / `qxdock` / `qxwindow` 一位未动——这不是遗漏，
是在声明「这些库本期没有接口变化」。推进规则的完整约定（含发布时的操作步骤）见
[`doc/ROADMAP-3.x.md` 附录 B](https://github.com/canpool/qtcanpool/blob/release-3.x/doc/ROADMAP-3.x.md)。

| 项目版本 | `qxcore` | `qxtheme` | `qxwindow` | `qxribbon` | `qxdock` | `qxapp` |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| 3.0.0 | 0.1.0 | 0.1.0 | 0.1.2 | 0.10.1 | 0.2.0 | 0.0.1 |
| 3.1.0 | 0.1.0 | 0.1.0 | 0.1.2 | 0.10.1 | 0.2.0 | 0.0.1 |
| 3.2.0 | 0.1.0 | 0.1.0 | 0.1.2 | 0.10.1 | 0.2.0 | 0.0.1 |
| 3.3.0 | 0.1.0 | 0.1.0 | 0.1.2 | 0.10.1 | 0.2.0 | 0.0.1 |

（`qcanpool` 有自己的一套号，且已于 3.2 整库删除，不再列入。）

四行里库号全未动，但内容一直在长：`qxcore` 多了 @ref QxCore::QxTranslator，`qxapp` 多了
@ref QxApp::QxToast / @ref QxApp::QxToastManager / @ref QxApp::QxPropertyEditor /
@ref QxApp::QxSettingsDialog / @ref QxApp::QxWorkspaceManager。这些内容按上面的规则会在
**下一批发布**时结进号里（`qxapp` 0.0.1 → 0.1.0、`qxcore` 0.1.0 → 0.1.1），而不是回头改写这里。

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
| [迁移指南](migration.md) | 2.x → 3.0 / 3.1 迁移：命名空间、类与方法对照、构建迁移 |

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
