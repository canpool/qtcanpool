# 架构与约定

本页说明 QtCanpool 的分层方式，以及**写代码时必须遵守的约定**。新增库或新增模块前建议先读一遍。

## 分层

依赖是单向的：上层可以依赖下层，下层永远不知道上层的存在。

```
qxapp      应用框架
           RibbonAppWindow / QxAppShell / QxNavigationBar / QxSplashScreen
           QxToast / QxToastManager / QxPropertyEditor / QxSettingsDialog
           依赖 qxribbon · qxdock · qxtheme · qxcore · qxplugin（qxwindow 为私有依赖）
   │
   ├── qxplugin  插件体系（契约 / 加载 / 依赖 / 隔离 / 对象池）──► qxcore
   ├── qxribbon  Ribbon 界面（页 / 分组 / 快捷工具栏）         ──► qxwindow
   ├── qxdock    可停靠窗口（布局 / 标签化 / 浮动容器）         ──► 仅 Qt
   ├── qxwindow  单窗口 / 无边框窗口基座                      ──► 仅 Qt
   ├── qxtheme   主题引擎（调色板 + 样式表）                   ──► qxcore
   └── qxcore    基础设施（配置 / 日志 / 语言）                ──► 仅 Qt Core
```

四点值得留意：

- `qxcore` 只依赖 Qt Core，因此任何库都可以用它，包括不涉及界面的模块——
  i18n（@ref QxCore::QxTranslator）正因此放在这一层，而不是放在 `qxapp`。
- `qxtheme` **刻意不依赖 `qxribbon`**：内置样式表位于 qxribbon 的资源包中，靠 Qt 的进程级资源
  在运行时按路径读取。这样 qxribbon 一侧零改动，非 Ribbon 应用也能使用主题引擎。
- 设置界面被**切成两半**：@ref QxApp::QxPropertyEditor 只管值、不知道值从哪来，
  @ref QxApp::QxSettingsDialog 才知道读写的是 @ref QxCore::QxSettings。
  于是同一张表单可以脱离配置单独用（例如对象属性面板）。
- **插件体系单独成层**（`qxplugin`）：它只依赖 `qxcore`（外加 Qt Widgets——`QxPluginContext`
  用 `QWidget` 讲页面与停靠面板），而 `qxapp` 反过来依赖它。插件只面向
  @ref QxPlugin::QxPluginContext 编程，永远不面向宿主窗口类，所以宿主类名对插件不可见
  （K14 已据此定案为**保留 `QxAppShell`**）。
  插件之间靠 @ref QxPlugin::QxObjectPool 相遇（谁在池里，谁才可达），**不靠互相链接**。

> ⚠️ `qxplugin → qxcore` 这条边**不是链接需要**：qxplugin 的 `.cpp` 不引用任何 `qxcore` 符号，
> 它来自 @ref QxPlugin::QxPluginContext 的 `settings()` 返回类型 `QxCore::QxSettings *`
> （出现在公开签名里，所以必须是 PUBLIC 依赖）。`qxcore` 一共三个类，qxplugin 只用到这一个。
> 归属与是否切掉这条依赖的记录见
> [`doc/design/4.0-TASKS.md`](https://github.com/canpool/qtcanpool/blob/master/doc/design/4.0-TASKS.md) 的 K19。

## 目录结构

| 目录 | 说明 |
| :--- | :--- |
| `cmake/` | CMake 构建框架（`QtCanpoolAPI.cmake` 是核心，提供下面那套函数） |
| `src/libs/` | 基础类库，每个库一个子目录，一个库一个命名空间（当前 6 个，见上） |
| `src/modules/` | 可拔插业务模块的**样板**与时兴能力的收容所（K16）。正式、稳定的能力进 `src/libs` |
| `src/plugins/` | 框架**自己的**功能插件（分层规则见 `src/plugins/CMakeLists.txt`）；业务插件写在自己的工程里 |
| `src/shared/` | 跨模块共享代码 |
| `demos/` | 综合示例（CMake） |
| `examples/` | 控件级示例（CMake，由 `WITH_EXAMPLES` 控制） |
| `tests/` | 单元测试（CTest） |
| `doc/` | 文档：设计文档、指南页面（`doc/pages/`）与文档站点构建（`doc/CMakeLists.txt`） |
| `projects/` | 项目模板，可在此持续添加自己的项目 |
| `scripts/` | 辅助脚本 |
| `thirdparty/` | 第三方库使用案例 |

## 命名空间

每个库拥有独立命名空间，由该库的 `*_global.h` 定义。**始终通过宏使用，不要手写 `namespace`**：

| 库 | 命名空间 | 宏前缀 |
| :--- | :--- | :--- |
| qxcore | `QxCore` | `QX_CORE_` |
| qxtheme | `QxTheme` | `QX_THEME_` |
| qxwindow | `QxWindow` | `QX_WINDOW_` |
| qxribbon | `QxRibbon` | `QX_RIBBON_` |
| qxdock | `QxDock` | `QX_DOCK_` |
| qxplugin | `QxPlugin` | `QX_PLUGIN_` |
| qxapp | `QxApp` | `QX_APP_` |

每个库提供四个宏：

| 宏 | 用途 |
| :--- | :--- |
| `QX_CORE_BEGIN_NAMESPACE` / `QX_CORE_END_NAMESPACE` | 头文件与源文件中包裹声明 |
| `QX_CORE_USE_NAMESPACE` | `using namespace QxCore;`，仅用于 `.cpp` |
| `QX_CORE_PREPEND_NAMESPACE(name)` | 需要显式限定名字时使用，例如 `QX_CORE_PREPEND_NAMESPACE(QxSettings) *` |

```cpp
#include "qxcore/qxsettings.h"

QX_CORE_BEGIN_NAMESPACE

class QxWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QxWidget(QWidget *parent = Q_NULLPTR);

    /*! The settings backing this widget; owned by it. */
    QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings() const;
    ...
};

QX_CORE_END_NAMESPACE
```

定义 `QX_CORE_NAMESPACE_DISABLE` 可以完全关闭命名空间（宏会展开为空），
因此所有跨库引用都必须写成宏，否则关闭命名空间后编译不过。

导出符号由 `QX_CORE_EXPORT` 一类宏控制：`add_qtc_library()` 会自动为库本身定义
`QX_CORE_LIBRARY`，消费方由此得到 `Q_DECL_IMPORT`。静态库场景下定义 `QX_CORE_LIBRARY_STATIC` 即可。

## d-pointer

公开类一律使用 d-pointer，把实现细节从公开头文件中挪走。宏家族在 `*_global.h` 中提供
（`QX_DECLARE_PRIVATE` 等在同一处定义，因此任何库都能直接用）：

| 宏 | 位置 | 作用 |
| :--- | :--- | :--- |
| `QX_DECLARE_PRIVATE(Class)` | 类尾部（`private:` 之后） | 声明 `ClassPrivate *d_ptr` 与 `Q_DECLARE_PRIVATE` |
| `QX_DECLARE_PUBLIC(Class)` | 私有类中 | 声明回指公开对象的 `q_ptr` |
| `QX_INIT_PRIVATE(Class)` | 构造函数 | 创建私有对象并回指 |
| `QX_FINI_PRIVATE()` | 析构函数 | 删除私有对象并置空 |
| `Q_Q(Class)` / `Q_D(Class)` | 成员函数 | 取得 `q` / `d` 指针 |

```cpp
// qxwidget.h
class QxWidgetPrivate;
class QX_CORE_EXPORT QxWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QxWidget(QWidget *parent = Q_NULLPTR);
    ~QxWidget() override;
private:
    Q_DISABLE_COPY(QxWidget)
    QX_DECLARE_PRIVATE(QxWidget)
};

// qxwidget.cpp
class QxWidgetPrivate
{
public:
    int count = 0;
};

QxWidget::QxWidget(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(QxWidget)
}

QxWidget::~QxWidget()
{
    QX_FINI_PRIVATE()
}
```

> **注意**：`QX_FINI_PRIVATE()` 后面要写分号。漏掉时 clang-format 会把析构函数体折成一行，
> 看起来像是格式问题，实际是少了一个 `;`。

## CMake 构建 API

新增库、示例、测试时使用框架提供的函数，不要手写 `add_library` / `add_executable`：
它们负责翻译、rpath、AUTOMOC、默认编译定义以及**自动安装与导出**。

| 函数 | 用途 |
| :--- | :--- |
| `add_qtc_library(name ...)` | 定义一个库 |
| `extend_qtc_library(target ...)` | 为已定义的库追加源文件或依赖（常用于按平台分支） |
| `add_qtc_executable(name ...)` | 定义一个可执行程序 |
| `add_qtc_test(name ...)` | 定义并注册一个 CTest 测试 |
| `add_qtc_documentation(qdocconf)` | qdoc 文档通道（遗留，当前未使用） |

`add_qtc_library()` 常用参数：

| 参数 | 说明 |
| :--- | :--- |
| `VERSION` / `COMPAT_VERSION` | 库版本，也决定安装目录与导出文件名 |
| `DEFINES` | 私有编译定义（库自身可见，通常写 `QX_CORE_LIBRARY`） |
| `PUBLIC_DEFINES` | 公开编译定义 |
| `DEPENDS` | 私有依赖（仅实现需要） |
| `PUBLIC_DEPENDS` | 公开依赖（**公开头文件里出现了该库的类型时必须用它**） |
| `INCLUDES` / `PUBLIC_INCLUDES` | 私有 / 公开包含目录 |
| `SOURCES` | 源文件列表 |
| `CONDITION` | 条件构建，不成立时整个库被跳过 |

```cmake
add_qtc_library(qxcore
  VERSION 0.1.0
  DEFINES QX_CORE_LIBRARY
  PUBLIC_DEPENDS Qt5::Core
  SOURCES
    qxcore_global.h
    qxlogger.cpp qxlogger.h
    qxsettings.cpp qxsettings.h
)
```

`add_qtc_test()` 的 `DEPENDS` 会在目标不存在时自动跳过该测试，
因此关闭某个组件开关时不需要同步修改测试列表。

## 源文件头

新文件使用 SPDX 标识，年份取实际创建年份；贡献者可在其后追加自己的版权行：

```cpp
/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
```

## 编码与格式规范

- **代码中的注释一律使用英文**；`.md` 等文档文件不受此限
- 源文件编码：全英文文件使用 UTF-8；包含中文的源文件使用 UTF-8 with BOM
- C++ 风格遵循 [Google C++ Style Guide](http://google.github.io/styleguide/cppguide.html) 与 Qt 风格
- 格式化由仓库根目录的 `.clang-format` 定义，**CI 中作为强制门禁**：

  ```bash
  clang-format --style=file --dry-run --Werror $(git ls-files 'src/**/*.h' 'src/**/*.cpp')
  ```

- 头文件使用 `#pragma once`，或与既有文件保持一致的宏卫哨
- Git 提交格式为 `type(scope): subject`，提交信息使用英文；`scope` 取库名、`ci`、`test`、`docs` 等

## 文档注释

公开 API 使用 Doxygen 风格注释，因此文档站点可以直接从源码生成：

- 文件级版权块用 `/** ... */`
- 类、枚举、函数用 `/*! ... */`，简短描述放在首行，细节另起段落
- 行内用 `\a name` 引用参数，`@code` / `@endcode` 包裹示例

```cpp
/*!
 * Adds a page at the end of the workspace and an entry with the same icon
 * and title to the rail. The id addresses the page in the configuration
 * file, so it has to be stable and unique.
 */
void addPage(const QString &id, const QIcon &icon, const QString &title, QWidget *page);
```
