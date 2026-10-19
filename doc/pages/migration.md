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
w->ribbonBar()->setRibbonStyle(RibbonBar::OfficeStyle);   // 4.5 起为 TabBarBelowStyle
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

`RibbonStyle` 枚举映射：`ClassicStyle` → `OfficeStyle`（4.5 起 `TabBarBelowStyle`），
`MergedStyle` → `WpsLiteStyle`（4.5 起 `TabBarInlineStyle`）；另新增 `OfficeStyleTwoRow`、
`WpsLiteStyleTwoRow`（4.5 起 `TabBarBelowTwoRowStyle`、`TabBarInlineTwoRowStyle`）。

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
| `TinyNavBar` | `QxNavTabBar` | `qcanpool/tinynavbar.h` | `qxapp/qxnavtabbar.h` |

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

> 少数只在 qmake 里存在、没有 CMake 对应物的示例被删除：`demos/helloworld`、`demos/thirdparty`；
> `examples/` 下的 23 个示例已补上 CMake 并恢复可构建。
>
> `demos/qxwindow` 也在同一个提交删除，但它在 **4.4 以当前 API 重建**回到了 `demos/`
> （目标 `QxWindowDemo`）。它不是「补一个 CMakeLists」就能回来的：原来那份依赖两个同期删掉的类
> （`WindowStyleAgent` 与 demo 本地的 `WindowBar` / `WindowButton`），重建的是同一个演示，
> 不是同一份源码。
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
| `RibbonMainWindow` | `QMainWindow` | `ribbonBar()` / `resizeRibbon()` / `windowStateChanged`（主题三件套 `setRibbonTheme`、`ribbonTheme`、`loadTheme` **4.5 已删除**，见「4.x 破坏性变更 → 4.5」） |
| `RibbonWindow` | `RibbonMainWindow` | **只有无边框**：`isFrameless` / `setFrameless`，实现是 `FramelessHelper` + `WindowButtonGroup` |
| `RibbonAppWindow`（`qxapp`） | `RibbonMainWindow` | **只有无边框**：实现是 qxwindow 的 `WindowAgentWidget` |

⇒ 后两者是**兄弟不是父子**：同一个 `RibbonMainWindow` 基类，换的只是无边框后端。
**`RibbonWindow` 去掉无边框之后就等于 `RibbonMainWindow`**——`ribbonBar()` 全在基类里，
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

## 5.0.0 将移除 Qt 5 支持（4.x 全程保留，现在可以做的准备）

> **这一节不是 4.3 的变更。** 4.x **全程保留 Qt 5 支持**（决策 K30），本版**没有删任何 Qt 5 代码**。
> 这里写的是"将来会断在哪、现在能怎么准备"，免得 5.0.0 发布时你才第一次看到。

### 会影响谁

| 你现在 | 5.0.0 之后 |
| :--- | :--- |
| 用 Qt 5.15 构建本库或自己的应用 | 必须切到 **Qt ≥ 6.0**（本库主推、也是 CI 基线的，是 6.8 LTS） |
| 只 include 本库公开头、链接 `QtCanpool::*` | **不受影响**——目标名与头文件路径都不变，变的是你 configure 时指向的 Qt |
| include 了 `qtcompat/qtcompat.h` | 见下一节 |
| 还在用 qmake | 4.0 起就没有 qmake 了，见上文「构建迁移」 |

**本库的用法不用改**：`find_package(QtCanpool)` + `target_link_libraries(app PRIVATE QtCanpool::qxribbon)`
这种写法在 Qt 6 下**逐字不变**。要动的是**你自己代码里的 Qt 5 用法**
（`QRegExp`、`QMouseEvent::globalPos()`、`QString::SplitBehavior` 那一类）。

### `qtcompat` 的去留

`qtcompat` 是**只为跨 Qt 5 / 6 而存在**的 header-only 兼容头，目前两个函数：

| `QtCanpoolCompat::` | 在 Qt 6 下的等价写法 |
| :--- | :--- |
| `globalMousePos(event)` | `event->globalPosition().toPoint()` |
| `hoverPos(event)` | `event->position().toPoint()` |

5.0.0 之后这两个函数没有存在理由。**现在就可以换成右边那一列**——
它在任何 Qt 6 上都能编译，换完你代码里就少一处将来要动的地方。

### 移除之后，本库的行为会变吗

**在 Qt 6 上不会变**：移除的是 Qt 5 那一半分支，Qt 6 这一半原样留下。

4.3 已经把全仓 Qt 版本分支**分类过一遍**（决策 K35）：88 行里 **30 处**是 `< 6.0` 分支，
也就是"支持 Qt 5 的真实成本"。它们的性质要说准：

- **大多数是编译期的 API 适配**——`globalPos()` → `globalPosition()`、
  `QHoverEvent::pos()` → `position()`、`nativeEventFilter` 的签名、
  `QEvent` → `QEnterEvent`、`QIcon::pixmap()` 的重载、`QLibraryInfo::location()` → `path()` 等；
- **少数几处是顺着 Qt 自己的语义变化**：`QApplication::globalStrut()` 在 Qt 6 上不再参与
  `sizeHint()`、`QExposeEvent::region()` 在 Qt 6 里被移除（本库改成一律按"有区域"处理）、
  `QFontMetrics` 的 int 与 `QFontMetricsF` 的浮点版本在 `lineSpacing()` 上的取整差异。

⇒ 后者**来自 Qt 的变化，不是本库在两版之间挑了不同的行为**。5.0.0 之后只剩 Qt 6 那一条路径，
你在 Qt 6 上看到的一切都不会变。

### 什么时候

**不设日期。** K32 定的三条触发条件（① 版本分支的分类表完成 ② 分类结果全部落在"可收敛"或"纯版本分支"
③ 至少一个 4.x 版本已在 Qt 6 上跑满）要**同时**满足，才宣布 Qt 5 进入"维护末期"，之后才谈移除。
在此之前 Qt 5 这条腿照常维护。

但请记住：**Qt 5.15 LTS 已完全 EOL**——标准支持 2023-05 结束，订阅用户的延长支持 2025-05 也到期，
**不会再有任何上游修复**（本库 CI 里那条 ubuntu + 5.15.2 的腿，测的是一个已停止支持的 Qt）。

### 现在能做的三件事（都不需要改本库）

1. **给自己的 CI 加一条 Qt 6 腿**：本库 4.x 两版都支持，随时可切，不必等到 5.0.0。
2. **把 `qtcompat` 的调用换成上表右边那一列。**
3. **留意 `QX_<LIB>_DEPRECATED_SINCE`**：4.3 修好了这个闸门（此前它对"已经发生的弃用"恒返回假，
   见本页上一节 / 决策 D11）。将来移除任何 API 都会先用它给足一个版本的告警期（K26）——
   **看到 deprecated 告警就是迁移点**。

## 分步迁移清单

1. 构建切换到 CMake，锁定 Qt ≥ 5.15（推荐 6.5 / 6.8）。
2. 全局替换 include：`qcanpool/ribbon*.h` → `qxribbon/*.h`（或 `qxapp/ribbonappwindow.h`）。
3. 替换命名空间：`QCanpool` → `QxRibbon` / `QxApp`；`QxWidget` → `QxApp`。
4. 主窗口基类：`QMainWindow` → `RibbonMainWindow`（无边框用 `RibbonWindow`）。
5. 按需 `setRibbonStyle(TabBarBelowStyle | TabBarInlineStyle | ...TwoRowStyle)`。
6. `RibbonGroup`：`addAction(icon, text, size)` → 先建 `QAction`，再用 `addLargeAction` 一族。
7. option 按钮：`setOptionButtonVisible` → `addOptionAction(QAction *)`。
8. 容器类：改用 `RibbonGroup::addWidget` / `addGallery` / `addSeparator`。
9. `QuickAccessBar` → `RibbonBar::quickAccessBar()`。
10. 主题：接入 @ref QxTheme::QxThemeManager 主题引擎（4.5 起 `RibbonTheme` 已删除，见下）。
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

## 4.x 破坏性变更

4.x 是长生命周期主版本（类 Qt 5.0→5.15），破坏性变更只在 minor 版本发生，并集中记录于此。

### 4.5

- **`QxNavBar` → `QxNavTabBar`**（库 `qxapp`，破坏 API / ABI）：`QxNavBar` 实为
  `QxTabBar` 派生、带「显示 / 隐藏标签页」定制菜单的 tab 式导航栏，与 `QxNavigationBar`
  （`QxAppShell` 用的竖直导轨）名字相近但基类、语义都不同。4.5 将其改名为 `QxNavTabBar`
  以消除歧义；`QxNavigationBar` 保持不变。
  - 迁移：全局替换 `QxNavBar` → `QxNavTabBar`，include `qxapp/qxnavbar.h` → `qxapp/qxnavtabbar.h`。
  - 旧名无转发别名、无弃用缓冲期（4.5 起直接不存在），需一次性改完。
- **`RibbonStyle` 的四个取值改名**（库 `qxribbon`，破坏源码 / 元对象）：`OfficeStyle` →
  `TabBarBelowStyle`、`WpsLiteStyle` → `TabBarInlineStyle`、`OfficeStyleTwoRow` →
  `TabBarBelowTwoRowStyle`、`WpsLiteStyleTwoRow` → `TabBarInlineTwoRowStyle`；谓词
  `isOfficeStyle()` → `hasOwnTabRow()`。**数值不变**（`0x0000` / `0x0001` / `0x0100 | …`）。
  - 为什么：这四个值本来就是两个正交选择的 2×2 组合——页签条是**独占标题行下方一行**还是
    **并入标题行**，以及组内小按钮摆三行还是两行。产品名既不描述这两件事，还让 `Lite` 与
    `TwoRow` 两个都表示「更小」的词互相打架：`OfficeStyleTwoRow` 与 `WpsLiteStyle` 的总高
    其实**完全相等**（差的那一行都来自「少一行内容」或「少一行页签」，只是来源不同）。
  - 迁移：全局替换四个枚举名与谓词名。存**数字**的配置（`QSettings` 的 int、`.ui` 的整型
    属性）照旧可用；存**字符串**的（QML、`QMetaEnum` 名、序列化的枚举名）需一并改。
  - 这也是**布局**风格（标题行与页签行怎么排、组内几行），与 `qxtheme` 的配色主题
    （`QxThemeInfo`）是两个层级，不要混为一谈。
- **`RibbonGallery` 的 `pageUp` / `pageDown` / `showMoreDetail` 保留**：原计划标为「空转虚函数」
  拟删除，核查后确认三者均有真实实现（驱动滚动视口 / 弹出详情）并已连到按钮，且类带导出宏，
  删除会同时破坏行为与 ABI，故 4.5 不做此变更。
- **`qxtheme` 不再内置主题**（库 `qxtheme`，破坏 API）：`Theme` 枚举与
  `themeId` / `themeName` / `themeFromId` / `isDarkTheme` / `themeStyleSheetPath` /
  `themeStyleSheet` 一族自由函数删除，`setStyleSheetFile()` 删除、
  `setStyleSheet()` 改为 `setStyleSheet(id, css)`（替换指定 id 那份注册的样式表），
  `palette(Theme)` → `paletteFor(bool dark)`。主题改由**应用注册**：
  `QxThemeInfo{id, name, dark, styleSheet}` + `QxThemeManager::registerTheme()`，
  全库按 **id 字符串**寻址（`setTheme(QString)`、`themeInfo(id)`、`themeIds()`）。
  引擎保留调色板下发、持久化、系统跟随与强调色。
  - 迁移：删掉枚举与那几个自由函数的使用；把「选哪个主题」改成注册 + 报 id。
  - **持久化本就存 id**，沿用旧 id（在应用侧重新注册到某套新样式表上）即可让老配置继续生效。
- **预置主题整套换掉**（仓库 `res/themes/`，破坏配置 id）：原来的 8 套历史样式表
  （`light_yellow` / `light_office2013` / `light_classic` / `light_fancy` / `light_fluent2016` /
  `light_fluent2019` / `dark_wps` / `dark_officeplus`）与 `ribbondemo` 自带的 `office2016blue`
  全部删除，改为 **5 套家族 × 明暗**：
  `classic_light` / `classic_dark` / `soft_light` / `soft_dark` / `flat_light` / `flat_dark` /
  `slate_light` / `slate_dark` / `accent_light` / `accent_dark`，配置 id 为 `classic-light` …
  `accent-dark`。
  - 为什么：旧集是逐年手改攒出来的，其中两套「Fluent 2016/2019」只是在同一色阶上取相邻一档，
    肉眼分辨不出；且浅色主题只有 ribbon 规则、暗色主题才有整套标准控件规则，风格并不成对。
  - 迁移：把 `setTheme("dark-officeplus")` 换成新 id（如 `soft-dark`）。
    从 4.4 升上来的旧配置 id 取不到，引擎会保持原样并告警一次；需要延续老选择的应用
    可自行把旧 id 注册到新的样式表上。
- **`RibbonTheme` 与窗口主题三件套删除**（库 `qxribbon`，破坏 API / ABI）：`RibbonTheme`
  类（含 `ThemeStyle` 枚举）、`RibbonMainWindow::setRibbonTheme()` / `ribbonTheme()` /
  `loadTheme()` 与 `ribbonTheme` Q_PROPERTY 全部移除；`RibbonMainWindow` 构造时
  **不再给自己装样式表**。库自带的 8 套 CSS 移出 `src/libs/qxribbon/res/stylesheets/`，
  落到仓库的 `res/themes/`（`res/themes.qrc` 挂到 `:/themes/`），由应用自行注册使用。
  - 为什么：窗口自带的那张是 **widget 级**的，而 Qt 样式表两边都匹配时 widget 级的赢，
    于是应用选了深色主题、Ribbon 区仍锁在浅色。窗口不带主题，应用下发的才到得了 Ribbon。
  - 迁移：`w->setRibbonTheme(RibbonTheme::DarkOfficePlusTheme)` →
    注册该主题后 `QxThemeManager::instance()->setTheme(QStringLiteral("soft-dark"))`；
    `RibbonTheme::loadTheme(file, w)` → 读文件后 `qApp->setStyleSheet(css)`。
    样式表换手后（widget → application）**Ribbon 区外观应保持一致**：这些 CSS 只描述
    Ribbon 部件，改到应用级不影响命中范围。
  - 参见 `examples/qxribbon/ribbonstyle`（应用侧注册 + 按 id 切换的完整示例）。
- **`DockWindow` 不再自带样式表**（库 `qxdock`，破坏「开箱即可见样式」）：`DockWindow`
  构造时不再读取并 `setStyleSheet()` 到自己身上。原来的 4 张 dock 样式表（default /
  focus_highlighting / 二者各带 `_linux`）从 `src/libs/qxdock/res.qrc` 移出，落到仓库
  `res/dockstyle/`（`res/dockstyle.qrc` 挂到 `:/dockstyle/`），由**应用自行编入并安装**。
  读取入口改为新增的 `qxdock::dockStyleSheet(bool focusHighlighting)`：它按
  `DockManager::FocusHighlighting` 与 `QX_DOCK_X11` 选好那张、读成文本返回，由应用
  `qApp->setStyleSheet(...)` 装上去——库只出文本、不替应用做决定。
  - 为什么：与 `RibbonTheme` 同形病灶——库自带的是 **widget 级**样式表，Qt 样式表两边都匹配时
    widget 级的赢，于是应用选了深色主题、Dock 区仍被库锁成浅色。样式表换手到 application
    后，Dock 外观跟随应用主题（CSS 内颜色本来就是 `palette(...)`，换深色 palette 自动跟着变；
    内置单色 SVG 图标另有 opt-in 着色助手 `qxdock::applyDockIcons(palette)` /
    `qxdock::refreshDockIcons(root)`，不调用则回落到本来就跟随主题的平台样式图标）。
  - 迁移：使用 qxdock 的应用需把 `res/dockstyle.qrc` 编入并安装——
    `qApp->setStyleSheet(qxdock::dockStyleSheet(DockManager::testConfigFlag(DockManager::FocusHighlighting)))`。
    仓库已替你接好两处：`demos/qxapp/dockdemo`（显式编入并 `setStyleSheet`）与
    `examples/CMakeLists.txt`（凡是 `add_qtc_example(..., qxdock, ...)` 的样本自动附加
    `res/dockstyle.qrc`）。库里删掉的 `DockWindowPrivate::loadStylesheet()` 无需任何替换。

### 4.6

- **`RibbonGalleryGroup` 改为自绘 `QWidget`**（库 `qxribbon`，破坏 API / ABI）：不再继承
  `QListView`。条目自己管理（action 列表 + 一个 `QActionGroup`）、自己算网格、自己画；
  悬停 / 选中的强调从 **palette** 取色（`Highlight` 洗色 + 描边），文字恒用 `Text` 色，
  不再反白。状态只改变洗色与描边，**不改变几何**——条目矩形与状态无关，指针进出不位移。
  弹出面板改为用同一套绘制逻辑画每个分组的完整网格（分组标题条），并支持滚动；
  在面板里选中另一分组的条目，strip 会切换到该分组（与 Office 行为一致）。
  - 保留：`GalleryGroupStyle` / `DisplayRow` / `setGridMinimumWidth` / `setGridMaximumWidth` /
    `recalcGridSize` / `selectByIndex` / `triggerCurrentItem` / `actionGroup` /
    `addItem(s)` / `setGroupTitle` 与 `groupTitleChanged` / `triggered` / `hovered` 三个信号。
  - 删除（原 `QListView` 泄漏的成员）：`model()` / `currentIndex()` / `scrollTo()` /
    `spacing()`（QListView 版）等。
  - 新增：`itemCount()` / `itemAction()` / `selectedIndex()` / `itemRect()` / `setSpacing()` /
    `gridSize()` / `rowCount()` / `columnCount()`，以及滚动一组
    `setViewportRows()` / `scrollByRows()` / `scrollToItem()` / `maxScrollRow()`。
  - `RibbonGallery::currentViewGroup()` 语义变化：返回 **strip 正在显示的那个分组本身**
    （旧实现返回一个内部镜像分组，与 `addGalleryGroup()` 返回的不是同一个对象）。
