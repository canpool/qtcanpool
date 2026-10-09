# 组件全览

本页按库列出能力范围、主要类与最小用法。类与函数的完整签名见各库对应的命名空间页面：

- qxcore → @ref QxCore
- qxtheme → @ref QxTheme
- qxwindow → @ref QxWindow
- qxribbon → @ref QxRibbon
- qxdock → @ref QxDock
- qxapp → @ref QxApp
- qcanpool → @ref QCanpool

## qxcore — 基础设施

只依赖 Qt Core，任何库都可以使用。

| 类 | 说明 |
| :--- | :--- |
| @ref QxCore::QxSettings | `QSettings` 的类型化包装：带默认值的读取、分组、以及带版本号的配置迁移 |
| @ref QxCore::QxLogger | 基于 Qt 消息处理器的日志：级别过滤、时间戳、按大小轮转的文件输出 |

```cpp
#include "qxcore/qxlogger.h"
#include "qxcore/qxsettings.h"

using namespace QxCore;

QCoreApplication::setOrganizationName(QStringLiteral("acme"));
QCoreApplication::setApplicationName(QStringLiteral("MyApp"));

QxLogger::Options options;
options.fileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                   + QStringLiteral("/myapp.log");
options.level = QxLogger::Info;
QxLogger::init(options);

QxSettings settings(QStringLiteral("acme"), QStringLiteral("MyApp"));
settings.beginGroup(QStringLiteral("ui"));
const int width = settings.intValue(QStringLiteral("width"), 1280);
settings.setValue(QStringLiteral("width"), width + 100);
settings.endGroup();
```

`QxSettings` 的迁移接口用于跨版本调整配置结构：注册若干 `(版本号, 回调)`，
调用 `migrate(targetVersion)` 时会**按版本升序**执行中间缺失的每一步，
已到达或超过目标版本时返回 0、不做任何事（因此不会"降级"）。

`QxLogger::init()` 会接管进程级消息处理器，并在 `shutdown()` 时恢复此前的处理器，
所以它与调用方自行安装的处理器可以共存，顺序为「先装者被后装者包裹」。

## qxtheme — 主题引擎

把「调色板」与「样式表」作为一件事统一应用。详见[主题引擎](theming.md)。

| 类 / 函数 | 说明 |
| :--- | :--- |
| @ref QxTheme::QxThemeManager | 进程级单例：应用主题、持久化选择、跟随系统深浅色 |
| @ref QxTheme::Theme | 内置主题枚举 |
| @ref QxTheme::themeId / themeName / themeFromId | 稳定 id、显示名与相互转换 |
| @ref QxTheme::isDarkTheme | 判断主题是否为深色 |
| @ref QxTheme::themeStyleSheetPath / themeStyleSheet | 内置样式表的资源路径与内容 |

```cpp
#include "qxtheme/qxthememanager.h"

using namespace QxTheme;

QxThemeManager *themes = QxThemeManager::instance();
themes->setTheme(DarkOfficePlus);
```

## qxwindow — 自定义窗口

无边框窗口与系统按钮代理，是 Ribbon 界面的底座。

| 类 | 说明 |
| :--- | :--- |
| `WindowAgent` / `WindowAgentBase` | 无边框窗口代理：拖动、缩放、系统按钮命中测试 |
| `WindowButtonGroup` | 一组最小化 / 最大化 / 关闭按钮 |
| `WindowContext` | 原生窗口上下文（Windows / Linux / macOS 各一份实现） |

`QWK` 时代的名字已全部并入 `QxWindow` 命名空间。

## qxribbon — Ribbon 界面

| 类 | 说明 |
| :--- | :--- |
| `RibbonBar` | Ribbon 主体：应用按钮、快捷工具栏、页与分组 |
| `RibbonPage` / `RibbonGroup` | 页与分组，分组内可放入大按钮、小按钮、下拉菜单 |
| `RibbonMainWindow` | 已装配好 RibbonBar 的主窗口 |
| `RibbonTheme` | widget 级样式表加载器（6 套内置样式表）。新代码请优先使用 @ref QxTheme::QxThemeManager |
| `RibbonGallery` / `RibbonCustomizeDialog` 等 | 选项板、自定义对话框等配套控件 |

样式表资源位于 `:/qxribbon/res/stylesheets/*.css`，是**进程级**资源；
`QxThemeManager` 正是按路径从这里取内置主题。

## qxdock — 可停靠窗口

| 类 | 说明 |
| :--- | :--- |
| `DockWindow` | 停靠区管理：`DockWidgetArea`、中央部件、状态保存与恢复 |
| `DockWidget` | 单个停靠面板（可移动、可浮动、可关闭） |
| `DockPanel` / `DockContainerWidget` / `DockAreaWidget` | 面板、容器与区域实现 |
| `DockFloatingWindow` | 浮动窗口 |

`DockWindow` 要求**中央部件第一个加入**：@ref QxApp::QxAppShell 在构造时就把页面栈装为中央部件，
其余停靠面板再围绕它排布。

## qxapp — 应用框架

| 类 | 说明 |
| :--- | :--- |
| `RibbonAppWindow` | 无边框 Ribbon 主窗口（RibbonBar + 窗口代理） |
| @ref QxApp::QxAppShell | 应用外壳：导航轨 + 页面栈 + 停靠区 + 状态栏 + 布局持久化。详见[应用外壳](appshell.md) |
| @ref QxApp::QxNavigationBar | 竖排互斥导航轨 |
| @ref QxApp::QxSplashScreen | 带进度与消息的启动屏 |

## qcanpool — 通用控件（legacy，已冻结）

提供标题栏、工具按钮、侧边栏等通用控件，并在历史上集成了后来的各个组件。
自 3.0 起该库**冻结**：其中的 legacy Ribbon 系列（`ribbonbar`、`ribbonpage`、`ribbongroup`、
`ribbonwindow`）不再演进，新代码请使用 `qxribbon`。详见[迁移指南](migration.md)。

## qtcompat — 跨版本兼容

header-only 的辅助头，抹平 Qt 5 与 Qt 6 的少数接口差异，例如鼠标全局坐标与悬浮位置。

```cpp
#include "qtcompat/qtcompat.h"

const QPoint global = QtCanpoolCompat::globalMousePos();
```
