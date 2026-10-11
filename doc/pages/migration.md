# 2.x → 3.x / 4.0 迁移指南

> 结论：3.0 **不保证二进制兼容**，提供源码级迁移路径。3.1 沿用同一方向做完了 legacy 收口，
> 3.2 把 `qcanpool` 库本身删除——**那 7 个转发头也没了**。4.0 移除 qmake，并新增插件体系
> （后者是**新增能力**，不写插件的工程无需改动）。

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
| `RibbonWindow : QMainWindow` | `RibbonMainWindow`（要无边框则 `QxApp::RibbonAppWindow`） | 以 `RibbonMainWindow` 为基类；`QxRibbon::RibbonWindow` 已于 4.3 删除 |
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
迁移成本就是改一处 include 加一处命名空间宏，而转发头需要在构建配置里多加一个 include 根。

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

> **注意**：qmake **已于 4.0 移除**（3.0 起冻结，4.0 删除），全树不再有
> `.pro` / `.pri`，CMake 是唯一的构建方式。

## 3.x → 4.0

4.0 做两件事：**移除 qmake**，和**引入插件体系**。前者对既有代码是清理，后者是**新增**能力——
不写插件的工程一行都不用改。

### qmake 彻底移除（K13）

全树 88 个 `.pro` / `.pri` 已删除（`git ls-files '*.pro' '*.pri'` 无输出），CMake 是唯一构建方式。

| 你会在迁移时碰到 | 怎么改 |
| :--- | :--- |
| 找不到 `*.pro` / `*.pri` | 按下文「构建迁移」一节改用 `find_package(QtCanpool)` |
| `projects/staticlink` 消失 | 静态链接改用 CMake 的 `find_package` + 目标链接 |
| `examples/` 下不再是 `.pro` | 新增根选项 `WITH_EXAMPLES`（默认 ON，与 `WITH_DEMOS` 并列） |
| `thirdparty/` 目录消失 | 它放的是 5 个 `.pri`（qmake 用于链接第三方库的配置文件，随 K13 删除）；第三方接入改用 CMake 的 `find_package` 或自定义 Find/Config 模块 |

> 少数只在 qmake 里存在、没有 CMake 对应物的示例被删除：`demos/helloworld`、`demos/qxwindow`、
> `demos/thirdparty`；`examples/` 下的 22 个示例已补上 CMake 并恢复可构建。
>
> `thirdparty/` 的第三方接入示例（boost / ffmpeg / opencascade）同样依赖这些 `.pri`，
> 其 `demos/thirdparty` 消费方在 4.0 删除；目录里残余的 README 在 **4.2 删除**——它们整篇讲的
> 都是那些已不存在的 `.pri`，留着只能指向空气。

### 新增 `qxplugin` 库

插件体系是 4.0 的新面，**不构成对既有代码的破坏性变更**。它带来：

- **插件契约** @ref QxPlugin::QxPlugin：`initialize` / `extensionsInitialized` / `shutdown`；
- **元数据加载**：放入插件目录即被认出，不写一句注册代码；
- **三种依赖**：required / optional / test，见[插件体系](plugins.md)；
- **失败隔离**：任一插件失败不阻止宿主启动；
- **宿主契约** @ref QxPlugin::QxPluginContext：页面 / 停靠 / 状态栏 / 通知 / 配置 / 对象池；
- **对象池** @ref QxPlugin::QxObjectPool：两个不共享头文件的插件协作的落点。

接口版本：`QX_PLUGIN_IID` = `"org.qtcanpool.QtCanpool.QxPlugin/1.0"`，
`QxPlugin::interfaceVersion()` 首版为 **1**；宿主可用 `QxPluginManager::setRequiredInterfaceVersion()`
拒绝过旧的插件。

> ✅ **`QxAppShell` 不改名（K14 已于 4.0 定案）**。插件面向的不是宿主类而是 `QxPluginContext`，
> 因此宿主类名对插件完全不可见——将来若真改名，影响面也只是宿主与它的样板，**任何插件都不用跟着改**。

> ⚠️ **`PLUGIN_RECOMMENDS` 不是 Qt Creator 的 `Recommends`**：我们用它生成 `Dependencies` 里
> `"Type" : "optional"` 的项（对应 Qt Creator 的 `PluginDependency::Type::Optional`）；而 Qt Creator 的
> `Recommends` 是"启用本插件时顺带**启用**它列出的插件"，与加载顺序无关。名字像，含义不同。

> ⚠️ **行为变化：optional 依赖现在是真排序**。4.0 之前它只被登记、不建边（等同"没声明"）；现在能解析时
> 它把供给方排到前面——这样消费方查得到对方已发布的东西；不能解析时静默跳过，且**不连带**消费方。

## 4.2 起弃用（升级提示）

这一节只记**已经标记弃用、但还没有删除**的东西。按 K26，删除要等至少一个版本，且必须先有替代路径
——所以下面每一条都带"换到哪里"。

**目前这一节是空的**：4.2 标记的两项弃用（`FramelessHelper` 与 `WindowStyleAgent`）**都已在 4.3 删除**，
见下一节。下面保留的是当时为什么那样决定的推理，因为 `RibbonWindow` 为什么一起走，光看删除结果看不出来。

### `FramelessHelper` 删除之后，`RibbonWindow` 怎么办

这不是一个能拖到删代码那天再想的问题，因为 `RibbonWindow` 与 `FramelessHelper` 是**同一件事**。
三者的真实差异（对着头文件核过）：

| 类 | 基类 | 比基类多出来的东西 |
| :--- | :--- | :--- |
| `RibbonMainWindow` | `QMainWindow` | `ribbonBar()` / `resizeRibbon()` / 主题（`setRibbonTheme`、`loadTheme`）/ `windowStateChanged` |
| `RibbonWindow` | `RibbonMainWindow` | **只有无边框**：`isFrameless` / `setFrameless`，实现是 `FramelessHelper` + `WindowButtonGroup` |
| `RibbonAppWindow`（`qxapp`） | `RibbonMainWindow` | **只有无边框**：实现是 qxwindow 的 `WindowAgentWidget` |

⇒ 后两者是**兄弟不是父子**：同一个 `RibbonMainWindow` 基类，换的只是无边框后端。
**`RibbonWindow` 去掉无边框之后就等于 `RibbonMainWindow`**——主题与 `ribbonBar()` 全在基类里，
它自己一点都没多。

**为什么当初要在 `qxribbon` 里自带一份无边框实现**，而不是直接用 `qxwindow`：为了让 `qxribbon`
保持自包含——它只链 Qt、不依赖 `qxwindow`，这条分层至今没破（见[架构与约定](architecture.md)）。
而"更好的无边框实现"也不能靠让 `qxribbon` 依赖 `qxwindow` 来落地，所以 `qxapp` 里才有了
`RibbonAppWindow`：它把依赖留在 `qxapp`，`qxribbon` 那一侧不受影响。

**所以答案是：这两者一起走。** 三种做法与各自代价：

| 做法 | 内容 | 代价 |
| :--- | :--- | :--- |
| **A（推荐）** | 删 `FramelessHelper` 时**一并删 `RibbonWindow`**，迁移写成"基类换成 `RibbonMainWindow`；要无边框就用 `qxapp` 的 `RibbonAppWindow`" | 破坏性，但是机械替换；`RibbonWindow` 的用户本来就是"要无边框的人"，他们真正该去的是 `RibbonAppWindow` |
| B | 保留 `RibbonWindow`、去掉无边框（`setFrameless` 消失，它退化成 `RibbonMainWindow` 的同义词） | 留下一个什么都没多的空壳类，只会让人误以为它还有用 |
| C | 给 `RibbonWindow` 换后端：让 `qxribbon` 依赖 `qxwindow`，或由 `qxapp` 往 `qxribbon` 注入一个后端 | 前者破坏上面那条分层；后者等于把 `RibbonAppWindow` 再实现一遍 |

**（4.3 A10）最后选了 A，并且没有等到 Qt 5 下线。** 上面那段"时机"当时是这么写的：

> `FramelessHelper` 在 Qt 5 上是**可用**的那条路径（Windows 上走原生后端），所以它必须活到
> Qt 5 支持结束，`RibbonWindow` 同理。换言之 **Qt 5 的下线日就是这两者的下线日**。

**这条被 4.3 推翻了**，推翻它的理由是具体核过的、不是改主意：`RibbonAppWindow` 并不是"Qt 6 才有"——
它依赖的是 `qxwindow`，而 `qxwindow` 的 Windows 原生后端**在 Qt 5.15 下照样编译**（本机 qt5 构建里
`windowcontext_win.cpp.obj` 就在）。也就是说 Qt 5 的用户换到 `RibbonAppWindow` 是**有路可走**的，
代价是多链 `qxapp`；既然如此，"等 Qt 5 下线"就只是在等一个并不存在的 blocker，
而 K26 的缓冲已经满了——继续留着就是"标了弃用却永不处理"。

## 4.3 已删除（K26 的缓冲已走完）

按 K26，弃用之后至少留一个版本才删。下面这些在 4.2 标弃用、**4.3 已经删掉**，
旧名字不再是告警而是"找不到头文件/符号"：

| 删除 | 替代 | 升级要做什么 |
| :--- | :--- | :--- |
| `WindowStyleAgent`（原 `qxwindow/style/`） | @ref QxTheme::QxThemeManager | 无。它默认**不编译**（要 `QX_WINDOW_CONFIG_STYLE_AGENT=ON`），全仓无人引用，`QX_WINDOW_CONFIG_STYLE_AGENT` 这个开关也一并去掉了。打开过开关的下游换成 `QxThemeManager`；它唯一多报的 `HighContrast` 一档没有等价物——真有需要时按新需求单独加，而不是恢复这个类 |
| `FramelessHelper`（原 `qxribbon/framelesshelper.h`）+ `RibbonWindow`（原 `qxribbon/ribbonwindow.h`） | 要无边框：@ref QxApp::RibbonAppWindow；不要无边框：`QxRibbon::RibbonMainWindow` | 基类换成 `RibbonMainWindow` 即可（两者只差"有无边框"，其余都在基类里）。要无边框就换 `RibbonAppWindow`——它在 Qt 5 与 Qt 6 上都成立，代价是多链 `qxapp`。为什么这两个一起删、以及为什么没等到 Qt 5 下线，见上一节 |

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
    `QCanpool::` 名字全部换掉，并从构建里去掉 `qcanpool`（`find_package` 的目标）。
    这一步之前旧名还能编（只是告警），之后连头文件都找不到。
16. **（4.0）** qmake 构建彻底移除。`.pro` / `.pri` 全部消失，CMake 成为唯一构建方式；
    仍在用 qmake 的工程按上文「构建迁移」一节改用 `find_package(QtCanpool)`。
    参考：[`projects/consume`](https://github.com/canpool/qtcanpool/tree/master/projects/consume)、
    [`projects/template`](https://github.com/canpool/qtcanpool/tree/master/projects/template)。
17. **（可选，4.0）** 接入[插件体系](plugins.md)：把可选的业务模块改写成插件，让宿主不再认识
    它由哪些模块组成。这是 4.0 的**新增**能力，不做也不影响升级。
