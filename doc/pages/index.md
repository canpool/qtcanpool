# QtCanpool

**一套总结自 Qt Creator 源码结构的 Qt Widgets 项目管理模板与组件库。**

QtCanpool 提供一整套可直接用于产品开发的界面基础设施：Ribbon 功能区、可停靠窗口、无边框自定义窗口、
应用外壳、主题引擎，以及配置与日志这类应用层必备设施。它的目标是让一个 Qt 桌面项目**第一天就具备**
成熟产品的骨架 —— 统一的项目组织方式、一致的构建与测试流程、开箱可用的界面组件。

[![CI](https://github.com/canpool/qtcanpool/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/canpool/qtcanpool/actions/workflows/ci.yml)
[![Docs](https://github.com/canpool/qtcanpool/actions/workflows/pages.yml/badge.svg?branch=master)](https://github.com/canpool/qtcanpool/actions/workflows/pages.yml)
[![Demo](https://img.shields.io/badge/Demo-online-2aa84a.svg)](https://canpool.github.io/qtcanpool/demo/)
[![License: MulanPSL-2.0](https://img.shields.io/badge/License-MulanPSL--2.0-blue.svg)](https://github.com/canpool/qtcanpool/blob/master/LICENSE)
[![Qt](https://img.shields.io/badge/Qt-5.15%20%7C%206.x-41CD52.svg)](https://www.qt.io/)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C.svg)](https://isocpp.org/)

## 为什么是 QtCanpool

| 关注点 | QtCanpool 的做法 |
| :--- | :--- |
| **项目组织** | 一套 CMake 框架管理多个项目：库、示例、插件、测试各有约定俗成的落点，新增模块只需一行 `add_subdirectory` |
| **构建与质量** | CMake 是唯一构建方式（qmake 已于 4.0 移除）；[clang-format](https://github.com/canpool/qtcanpool/blob/master/.clang-format) 作为 CI 强制门禁；CTest 单元测试矩阵覆盖 Qt 5.15 与 Qt 6 |
| **界面组件** | Ribbon、Dock、无边框窗口三类现代桌面界面范式，外加 Fluent / Material 两套自绘控件集（4.7 新增），均可在 Qt 5 与 Qt 6 上编译 |
| **应用骨架** | @ref QxApp::QxAppShell 把导航轨、页面栈、停靠区、状态栏与启动屏组装成一个可直接继承的窗口 |
| **布局与工作区** | `saveLayout()` 记下窗口几何与停靠布局；@ref QxApp::QxWorkspaceManager 在此之上给排列起名字，可存多套并随时切换（3.3） |
| **主题** | @ref QxTheme::QxThemeManager 统一应用调色板与样式表，支持运行时切换、选择持久化与跟随系统深浅色 |
| **基础设施** | @ref QxCore::QxSettings 负责配置与版本迁移，@ref QxCore::QxLogger 负责带轮转的日志，@ref QxCore::QxTranslator 负责语言切换 |
| **插件与模块** | 应用与「它由哪些模块组成」分开：模块以插件形式在运行时被发现、排序、加载，宿主源码里不出现任何模块名（4.0，见[插件体系](plugins.md)） |
| **开箱即用** | 生成的工程（见 `scripts/new-project`）已接好切语言、@ref QxApp::QxToast 应用内通知与 @ref QxApp::QxSettingsDialog 设置界面 |

## 组件总览

每个组件是一个独立的库、独立的命名空间，可以按需取用。完整说明见 [组件全览](components.md)。

| 组件 | 命名空间 | 说明 | 依赖 |
| :--- | :--- | :--- | :--- |
| `qxcore` | @ref QxCore | 基础设施：配置（`QxSettings`）、日志（`QxLogger`）、语言切换（`QxTranslator`） | Qt Core |
| `qxtheme` | @ref QxTheme | 主题引擎：调色板 + 样式表统一应用、运行时切换、持久化、跟随系统 | `qxcore`、Qt Widgets |
| `qxcontrols` | @ref QxControls | 控件基座：状态机、状态层、描边、投影、无障碍桥（两套控件集的共用层，4.7 新增） | `qxtheme`、Qt Widgets |
| `qxfluent` | @ref QxFluent | Fluent 风格控件：开关、滑块、徽标、卡片、进度环、评分、揭示等（4.7 新增） | `qxcontrols`、Qt Widgets |
| `qxmaterial` | @ref QxMaterial | Material 风格控件：开关、滑块、徽标、筹码、卡片、分段按钮、FAB、涟漪等（4.7 新增） | `qxcontrols`、Qt Widgets |
| `qxwindow` | @ref QxWindow | 自定义窗口：无边框窗口、系统按钮代理、原生窗口上下文 | Qt Widgets |
| `qxribbon` | @ref QxRibbon | Ribbon 风格界面：菜单栏 / 页 / 分组 / 快捷工具栏 | Qt Widgets |
| `qxdock` | @ref QxDock | 可停靠窗口：布局管理、标签化面板、浮动容器 | Qt Widgets |
| `qxplugin` | @ref QxPlugin | 插件体系：插件契约与元数据、加载顺序与失败隔离、宿主上下文（`QxPluginContext`）、对象池（`QxObjectPool`） | `qxcore`、Qt Widgets |
| `qxapp` | @ref QxApp | 应用框架：`RibbonAppWindow`、`QxAppShell`、`QxNavigationBar`、`QxSplashScreen`、应用内通知（`QxToast`）、设置界面（`QxPropertyEditor` / `QxSettingsDialog`）、插件管理器界面（`QxPluginManagerDialog`） | `qxribbon`、`qxdock`、`qxtheme`、`qxcore`、`qxplugin` |
| `qtcompat` | — | Qt 5 / Qt 6 跨版本兼容辅助头（header-only） | Qt Core |

> **注意**：`qcanpool` 已于 **3.2 删除**。它在 3.1 已被清空（legacy Ribbon 系列物理移除、
> 11 个遗留控件下线、7 个通用控件迁入 `qxapp` 并改名），只剩 7 个带弃用告警的转发头；
> 3.2 连这个库一起消失，**旧名不再存在**。详见 [迁移指南](migration.md)。

## 版本对照

各库当前版本（**4.6.0** 结算后；该版本已发版，tag `4.6.0`）：

| `qxcore` | `qxtheme` | `qxwindow` | `qxribbon` | `qxdock` | `qxplugin` | `qxapp` |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| 0.2.0 | 0.2.1 | 0.3.2 | 0.14.0 | 0.3.1 | 0.3.0 | 0.2.1 |

4.7 新增的三个库（`qxcontrols` / `qxfluent` / `qxmaterial`）不在这张表里：它们到 4.7 才存在，
首次结算随 4.7 发版进入表中，库号从 `0.1.0` 起。

**项目版本**（tag 打在它上面）是发布列车号，**每个库另有自己的版本号**，两者互不绑定（K11）：
每次项目发版结算一次，这一批里有修复或增量的库才前进号。历史各版本与各库号的完整对照、
以及每一行为什么动 / 为什么不动，见[版本对照表](versions.md)。

⚠️ **本项目的 `patch` 含新增能力**（K36，2026-10-22 作者拍板）：加类、加函数、加带默认值的
参数、甚至新增库，都进 patch；**只有破坏性变更才升 minor**。反过来，**兼容性由项目版本承诺**——
同一个项目 minor 系列（如 `4.7.x`）内，所有库的公开 API 只增、不减、不改签名，
要废弃先标 `QX_<LIB>_DEPRECATED_SINCE`。库号仍是 0.x、仍按 K11 独立演进。
详见[版本对照表](versions.md)。

## 快速开始

```bash
# 配置（通过 CMAKE_PREFIX_PATH 指定 Qt）
cmake -S . -B build -DCMAKE_PREFIX_PATH=<Qt安装目录>/<版本>/<编译器> -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build build --config Release --parallel

# 运行单元测试
ctest --test-dir build -C Release --output-on-failure
```

编译产物中的示例程序位于 `bin/`，例如 `RibbonDemo`、`DockDemo`、`ControlGallery`（fluent / material
两套新控件集的画廊）、`AppShellDemo`。
完整的构建说明与开关列表见 [构建与安装](build.md)。

### 在线体验

`AppShellDemo` 已编译为 WebAssembly，可直接在浏览器中打开，无需安装 Qt 或编译器：

<https://canpool.github.io/qtcanpool/demo/>

它展示了一个完整的应用外壳：侧边导航、页面切换、可停靠面板、主题切换与布局持久化，全部运行在
`QxAppShell` 之上。

## 文档导览

| 页面 | 内容 |
| :--- | :--- |
| [构建与安装](build.md) | 环境要求、CMake 构建、开关、测试与文档站点构建（含贡献者构建须知） |
| [架构与约定](architecture.md) | 库分层、命名空间与 d-pointer 约定、CMake API、编码规范、新增库约定、布局判据 |
| [CI 与在线站点](ci.md) | 两条工作流的结构、格式/SDK/质量/tidy 门禁、平台坑与查看 CI 结果 |
| [组件全览](components.md) | 各组件的能力范围、主要类与最小用法 |
| [控件集](controls.md) | 4.7 两套控件（Fluent / Material）与 Ribbon 的关系、共用与各自实现的判据、概念映射 |
| [主题引擎](theming.md) | 主题引擎：应用侧注册、运行时切换、跟随系统、平台强调色与自定义主题 |
| [图标规范](icons.md) | 图标为什么必须分类登记、两套着色机制的差别、规范色与 `scripts/check-icons.py` 门禁 |
| [界面语言](i18n.md) | 库自带的译文：目录在哪里、应用怎么装、怎么加一门语言，以及「私有类上下文」那条到不了的坑 |
| [应用外壳](appshell.md) | 应用外壳：页面、停靠区、状态栏、布局持久化、启动屏 |
| [插件体系](plugins.md) | 插件开发指南：元数据、三种依赖、生命周期、对象池、失败隔离 |
| [迁移指南](migration.md) | 2.x → 3.x / 4.0 迁移：命名空间、类与方法对照、构建迁移、插件条目 |
| [已知限制](limitations.md) | 知道但不打算改的行为：现象、成因、为什么保持现状、要改变需要什么 |
| [文档站点（Doxygen）](doxygen.md) | 独立 Doxygen 工程：构建、Doxyfile 关键项、样式表与 `Q_OS_*` 坑 |
| [发布流程](release.md) | 发一次版本的全部动作：两道闸门、版本号结算、三本账、推送与 Release |
| [版本对照表](versions.md) | 项目版本与各库版本号的历史对照，及每一行为什么动 / 为什么不动 |
| [管理规范](contributing.md) | 过程规范的单一权威来源：提交、文档（台账＋CHANGELOG）、编码、SPDX、构建验证、版本与进号、命名与敏感信息 |

## 环境要求

| 项目 | 要求 |
| :--- | :--- |
| Qt | **5.15 及以上**（主推 Qt 6.8 / 6.12 LTS；CI 矩阵覆盖 5.15.2 / 6.8.1 / 6.12.0） |
| C++ | C++17 |
| CMake | 3.16 及以上（推荐最新版本；使用 CMakePresets 需 3.25+） |
| 编译器 | 随 Qt 分发的 MinGW；MSVC 2019+；GCC 9+；Apple Clang |
| Doxygen（可选） | 1.9 及以上，仅构建文档站点时需要 |
| Graphviz（可选） | 提供 `dot` 后类继承图才会生成 |

## 仓库与许可

- GitHub：<https://github.com/canpool/qtcanpool>
- Gitee：<https://gitee.com/icanpool/qtcanpool>
- 使用教程：<https://blog.csdn.net/canpool/category_10631139.html>
- 许可：MulanPSL-2.0（集成组件遵循各自许可，见 `LICENSE.NOTES.md`）
