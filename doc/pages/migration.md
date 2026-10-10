# 2.x → 3.0 / 3.1 / 3.2 迁移指南

> 结论：3.0 **不保证二进制兼容**，提供源码级迁移路径。3.1 沿用同一方向做完了 legacy 收口，
> 3.2 把 `qcanpool` 库本身删除——**那 7 个转发头也没了**。

本页是迁移总览。完整的类与方法对照表、`fancy*` 控件存废表与分步清单见仓库中的
[`doc/design/3.0-MIGRATION.md`](https://github.com/canpool/qtcanpool/blob/master/doc/design/3.0-MIGRATION.md)。

## 3.1 / 3.2 带来的四处变化（升级前必读）

3.1 与 3.2 的 legacy 收口是**连续四个破坏性变更**，性质各不相同：

| 变更 | 你会在编译时看到 | 怎么改 |
| :--- | :--- | :--- |
| **A3** legacy ribbon 物理移除 | 找不到 `qcanpool/ribbon*.h` | 见下方「Ribbon 迁移要点」 |
| **A1** 7 个通用控件迁入 `qxapp` 并改名 | **弃用告警**（旧名仍可编译，缓冲期到 3.2 结束） | 见下方「`qcanpool` → `qxapp`」 |
| **A2** 11 个遗留类物理删除 | 找不到头文件，**无同名替代** | 见下方「已下线的遗留类」 |
| **R1（3.2）** `qcanpool` 整库删除 | 找不到 `qcanpool/*.h`——**连 A1 的转发头也没了**，旧名不再存在 | 换成上表「新头」那一列；`LIBS`/`find_package` 里去掉 `qcanpool` |

> **A1 与 A2 的区别是关键**：A1 之后旧代码还能编（只是告警），有一个版本的时间慢慢改；
> A2 之后不能编，必须一次改完。**3.2 的 R1 连 A1 留下的缓冲期一起结束了**。

## 迁移总览

| 维度 | 2.x | 3.0 | 迁移动作 |
| :--- | :--- | :--- | :--- |
| 命名空间（ribbon） | `QCanpool` | `QxRibbon` | 换命名空间 |
| 命名空间（window） | `QCanpool` / `QxWindow` | `QxWindow` | 统一切到 `QxWindow` |
| 命名空间（app） | `QxWidget` | `QxApp` | 换命名空间 |
| 头文件（ribbon） | `qcanpool/ribbonbar.h` | `qxribbon/ribbonbar.h` | 换 include |
| 构建 | qmake 为主 | CMake 为主 | 迁移到 `find_package` |
| C++ 标准 | 14（qmake）/ 17（cmake） | 17 | 统一 |
| Qt | 5.12 ~ 6.8 | 5.15（尽力）/ 6.5、6.8 | 升级 Qt |
| Ribbon 实现 | `qcanpool` 内 legacy | `qxribbon` 唯一 | 见下 |

## 命名空间与头文件

| 库 | 2.x | 3.0 | 启用宏 |
| :--- | :--- | :--- | :--- |
| qcanpool | `QCanpool` | `QCanpool`（不变；**3.2 整库删除**） | `QCANPOOL_BEGIN_NAMESPACE` |
| qxribbon | `QxRibbon` | `QxRibbon` | `QX_RIBBON_BEGIN_NAMESPACE` |
| qxwindow | `QxWindow` | `QxWindow` | `QX_WINDOW_BEGIN_NAMESPACE` |
| qxdock | `QxDock` | `QxDock` | `QX_DOCK_BEGIN_NAMESPACE` |
| qxwidget → **qxapp** | `QxWidget` | **`QxApp`** | `QX_APP_BEGIN_NAMESPACE` |

```cpp
// 2.x
#include "qcanpool/ribbonwindow.h"
using namespace QCanpool;
RibbonWindow *w = new RibbonWindow;
w->ribbonBar()->setRibbonStyle(RibbonBar::ClassicStyle);

// 3.0
#include "qxribbon/ribbonmainwindow.h"
using namespace QxRibbon;
RibbonMainWindow *w = new RibbonMainWindow;
w->ribbonBar()->setRibbonStyle(RibbonBar::OfficeStyle);
```

导出宏由 `QCANPOOL_SHARED_EXPORT` 拆分为 `QX_RIBBON_EXPORT` / `QX_WINDOW_EXPORT` / `QX_APP_EXPORT`。

> **注意**：不要同时展开 `QxRibbon` 与 legacy `QCanpool` 两个命名空间 —— 类名相同（如 `RibbonBar`）
> 会产生歧义。跨库引用请显式限定，例如 `QxRibbon::RibbonBar`。

## Ribbon 迁移要点

| legacy（`QCanpool`） | qxribbon（`QxRibbon`） | 说明 |
| :--- | :--- | :--- |
| `RibbonBar : QWidget` | `RibbonBar : QMenuBar` | 基类变化 |
| `RibbonWindow : QMainWindow` | `RibbonMainWindow` / `RibbonWindow` | 推荐以 `RibbonMainWindow` 为基类 |
| `RibbonContainer` 等四个容器 | `RibbonGridContainer` / `RibbonCtrlContainer` | **语义不同，需重写** |
| `QuickAccessBar` | `RibbonQuickAccessBar` | 由 `RibbonBar::quickAccessBar()` 获取 |

`RibbonStyle` 枚举映射：`ClassicStyle` → `OfficeStyle`，`MergedStyle` → `WpsLiteStyle`；
另新增 `OfficeStyleTwoRow`、`WpsLiteStyleTwoRow`。

`RibbonGroup` 的尺寸枚举由 `GroupSize{GroupLarge,GroupMedium,GroupSmall}`
改为 `RowProportion{Auto,Large,Medium,Small}`，并新增
`addLargeAction` / `addMediumAction` / `addSmallAction` 等便捷方法：

```cpp
// 2.x
RibbonGroup *g = page->addGroup("File");
g->addAction(new QAction(iconNew, "New", this), RibbonGroup::GroupLarge);
g->setOptionButtonVisible(true);
connect(g, &RibbonGroup::optionClicked, this, &MainWindow::onOption);

// 3.0
RibbonGroup *g = page->addGroup("File");
g->addLargeAction(actionNew);
g->addOptionAction(actionOption);
connect(actionOption, &QAction::triggered, this, &MainWindow::onOption);
```

## `qcanpool` → `qxapp`：7 个通用控件改名（A1）

3.1 把这 7 个控件迁入 `qxapp` 并去掉 `Fancy`/`Tiny` 前缀。**行为未变**，改动通常只是替换
标识符与 include。旧名当年在 `qcanpool` 保留了一个版本（指向新类的子类 + 弃用标注），
**该缓冲期已在 3.2 结束**：库与转发头一并删除，旧名不再存在，只剩下面这张对照表。

| 旧名（`QCanpool::`） | 新名（`QxApp::`） | 旧头（3.2 已删除） | 新头 |
| :--- | :--- | :--- | :--- |
| `FancyToolButton` | `QxToolButton` | `qcanpool/fancytoolbutton.h` | `qxapp/qxtoolbutton.h` |
| `ExtensionButton` | `QxExtensionButton` | `qcanpool/extensionbutton.h` | `qxapp/qxextensionbutton.h` |
| `MenuButton` | `QxMenuButton` | `qcanpool/menubutton.h` | `qxapp/qxmenubutton.h` |
| `MenuAccessButton` | `QxMenuAccessButton` | `qcanpool/menuaccessbutton.h` | `qxapp/qxmenuaccessbutton.h` |
| `TinyTabBar` | `QxTabBar` | `qcanpool/tinytabbar.h` | `qxapp/qxtabbar.h` |
| `TinyTabWidget` | `QxTabWidget` | `qcanpool/tinytabwidget.h` | `qxapp/qxtabwidget.h` |
| `TinyNavBar` | `QxNavBar` | `qcanpool/tinynavbar.h` | `qxapp/qxnavbar.h` |

> ⚠️ `TinyTabBar` 现在是**独立的另一个类型**（派生自 `QxTabBar` 的子类），不再是别名。
> 凡是把旧类型互相传递、或接收 `TinyTabWidget::tabBar()` 返回值的代码需要改，
> 多数情况下把变量类型改成 `QxTabBar *` 或 `auto *` 即可。

## 已下线的遗留类（A2，**无同名替代**）

| 已删除（`QCanpool`） | 替代 |
| :--- | :--- |
| `QuickAccessBar` | `QxRibbon::RibbonQuickAccessBar`（经 `RibbonBar::quickAccessBar()`） |
| `FancyTitleBar`、`WindowLogo`、`WindowToolBar` | `QxWindow` 的无边框窗口方案 |
| `MiniTabBar`、`MiniTabWidget` | `QxApp::QxTabBar` / `QxTabWidget` |
| `FancyBar`、`FancyTabBar`、`FancyTabWidget` | `QxApp::QxTabBar` / `QxTabWidget` |
| `FancyWindow`、`FancyDialog` | `QxWindow::` 或 `QxApp::RibbonAppWindow` / `QxAppShell`；对话框直接用 `QDialog` |

## legacy 头文件的弃用标注（3.2 起已全部消失）

3.0 时 5 个 legacy ribbon 头文件（8 个类）曾被标注弃用；**3.1 中这些文件已整体删除**，
那些名字不再产生告警，而是直接编译失败。

3.1 时仍带标注的是上面 7 个转发头：用旧名编译会得到
`'Xxx' is deprecated: use QxApp::QxXxx instead`（**在编译 `qcanpool` 库自身时该标注是空的**，
所以它只对库的使用者响）。**3.2 已随整库删除**，`QCANPOOL_DEPRECATED_*` 系列宏与
`QCANPOOL_DISABLE_DEPRECATED_BEFORE` 一起不复存在——升级后旧名是"找不到头文件"，
不再是告警。

> **注意**：与 Qt 官方惯例的一处有意差异 —— 本项目的标注只控制**弃用提示**，
> 类声明本身始终保留，所以当年调整宏只会开关告警、不会让 API 消失。
> 删除这些转发头走的是破坏性变更流程（3.2 的 R1），没有用这个宏偷偷实现。

## qxwidget → qxapp

| 维度 | 2.x | 3.0 |
| :--- | :--- | :--- |
| 目录 | `src/libs/qxwidget/` | `src/libs/qxapp/` |
| 头文件 | `qxwidget_global.h` | `qxapp_global.h` |
| 命名空间 | `QxWidget` | `QxApp` |
| 宏 | `QX_WIDGET_*` | `QX_APP_*` |
| 主类 | `RibbonAppWindow` | `RibbonAppWindow`（类名不变） |

3.0 **不提供转发兼容头**：`qxwidget` 的公开面只有 `RibbonAppWindow` 一个类，
迁移成本就是改一处 include 加一处命名空间宏，而转发头需要在 CMake 与 qmake 两套构建里
各加一个 include 根。

```cpp
#include "qxwidget/ribbonappwindow.h"   // 2.x
QX_WIDGET_USE_NAMESPACE

#include "qxapp/ribbonappwindow.h"      // 3.0
QX_APP_USE_NAMESPACE
```

## 构建迁移：qmake → CMake

```cmake
# 3.0（单一 CMake 包，所有库共用 QtCanpool 命名空间）
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(QtCanpool REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE QtCanpool::qxapp)
```

> 公开头文件与 `QtCanpoolConfig.cmake` 位于 `Devel` 组件，安装时需
> `cmake --install <build> --component Devel`。

Qt 查找推荐使用兼容写法，一份工程同时支持两个大版本：

```cmake
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)
```

> **注意**：qmake 自 3.0 起**冻结**，`.pro` / `.pri` 不再新增特性，仅随 CMake 同步编译可用。

## 分步迁移清单

1. 构建切换到 CMake，锁定 Qt ≥ 5.15（推荐 6.5 / 6.8）。
2. 全局替换 include：`qcanpool/ribbon*.h` → `qxribbon/*.h`（或 `qxapp/ribbonappwindow.h`）。
3. 替换命名空间：`QCanpool` → `QxRibbon` / `QxApp`；`QxWidget` → `QxApp`。
4. 主窗口基类：`QMainWindow` → `RibbonMainWindow`（无边框用 `RibbonWindow`）。
5. 按需 `setRibbonStyle(OfficeStyle | WpsLiteStyle | ...TwoRow)`。
6. `RibbonGroup`：`addAction(icon, text, size)` → 先建 `QAction`，再用 `addLargeAction` 一族。
7. option 按钮：`setOptionButtonVisible` → `addOptionAction(QAction *)`。
8. 容器类：改用 `RibbonGroup::addWidget` / `addGallery` / `addSeparator`。
9. `QuickAccessBar` → `RibbonBar::quickAccessBar()`。
10. 主题：接入 @ref QxTheme::QxThemeManager 主题引擎（或 `RibbonTheme::loadTheme`）。
11. 清理 `fancy*` 遗留调用。
12. 编译告警清零 —— deprecated 提示即迁移点。
13. **（3.1）** 7 个改名控件换成 `QxApp::Qx*`；注意 `TinyTabBar` 现在是独立类型。
14. **（3.1）** `FancyWindow` / `FancyDialog` / `MiniTab*` / `WindowToolBar` / `WindowLogo`
    无同名替代，按「已下线的遗留类」表换成 `QxWindow` / `QxAppShell` / `QxApp::QxTab*`。
15. **（3.2）** `qcanpool` 已整库删除。**最后一步**：把剩下的 `qcanpool/*.h` include 与
    `QCanpool::` 名字全部换掉，并从构建里去掉 `qcanpool`（`find_package` 的目标、
    qmake 的 `LIBS` / `include(...)`）。这一步之前旧名还能编（只是告警），
    之后连头文件都找不到。
