# 应用外壳

`QxAppShell` 是 Ribbon 应用的窗口骨架。它本身就是 `RibbonAppWindow`（已带 RibbonBar、
窗口代理与状态栏），并补上每个应用都要手搭的三块：

```
┌───────────────────────────────────────────────────────────────┐
│ RibbonBar（页 / 分组 / 快捷工具栏）                             │
├────────┬──────────────────────────────────────────────────────┤
│ 导航轨  │  页面栈（QStackedWidget，工作区中央）                   │
│ QxNav  │                                                      │
│ igation│  ┌── 停靠面板（左/右/上/下，可移动、可浮动、可关闭）──┐  │
│ Bar    │  └────────────────────────────────────────────────┘  │
├────────┴──────────────────────────────────────────────────────┤
│ 状态栏：消息行 · 忙碌进度                                        │
└───────────────────────────────────────────────────────────────┘
```

其中**页面栈是 DockWindow 的中央部件**：`qxdock` 要求中央部件第一个加入，
所以外壳在构造时就把它装好并隐藏了中央面板的标题栏，其余停靠面板再围绕它排布。
页面因此是固定的工作区，而四周的停靠面板保持可移动、可浮动、可关闭。

## 典型用法

```cpp
#include "qxapp/qxappshell.h"

using namespace QxApp;

QxAppShell shell;
shell.setWindowTitle(tr("My Application"));

// 页面：稳定 id、图标、标题、部件
shell.addPage(QStringLiteral("home"), homeIcon, tr("Home"), new HomePage);
shell.addPage(QStringLiteral("log"), logIcon, tr("Log"), new LogPage);

// 停靠面板：区域、稳定 id、标题、部件
shell.addDock(Qx::LeftDockWidgetArea, QStringLiteral("outline"), tr("Outline"), new OutlineView);
shell.addDock(Qx::BottomDockWidgetArea, QStringLiteral("output"), tr("Output"), new OutputView);

shell.setStatusMessage(tr("Ready"));

// 页面与停靠面板都建好之后，再恢复布局
shell.restoreLayout();
shell.show();
```

## 页面

| 方法 | 说明 |
| :--- | :--- |
| `addPage(id, icon, title, page)` | 追加页面，并在导航轨上添加同名条目 |
| `insertPage(index, id, icon, title, page)` | 在指定位置插入 |
| `removePage(id)` | 移除页面与对应条目 |
| `pageCount()` / `indexOfPage(id)` / `pageId(index)` / `page(id)` | 查询 |
| `currentPageIndex()` / `currentPageId()` / `currentPage()` | 当前页 |
| `setCurrentPage(index)` / `setCurrentPage(id)` | 切换（越界或未知 id 为 no-op） |

四个必须记住的语义：

1. **页面按稳定 id 存放**，不存索引、不存标题。插入页面会平移索引，标题会被翻译，
   而 id 会写进配置文件，所以它必须稳定且唯一。
2. **`currentPageChanged(index, id)` 报告的是"可见页的 (索引, id) 对"**，
   因此插入或删除造成的**纯索引平移也会发信号** —— 调用方应按 id 判断，而不是按索引。
3. **`removePage()` 不删除部件**，只是把它从外壳取出（`setParent(nullptr)`），所有权交还调用方。
4. 加第一个页面时它成为当前页；之后新增页面不会抢焦点。

## 停靠面板

```cpp
QX_DOCK_PREPEND_NAMESPACE(DockWidget) *dock =
    shell.addDock(Qx::RightDockWidgetArea, QStringLiteral("props"), tr("Properties"), new PropView);
```

- 区域取 `Qx::DockWidgetArea`（左/右/上/下/中央）。
- **id 会成为部件的 objectName**，`dock(id)` 与持久化的布局都依赖它；id 为空或重复时返回 `nullptr`。
- 面板内容的所有权归面板，外壳拥有面板本身。

## 状态栏

```cpp
shell.setStatusMessage(tr("Loaded 120 files"));
shell.setBusy(true);      // 忙碌指示条
shell.setBusy(false);
```

忙碌指示是一个**没有百分比**的进度条：外壳只负责表达"正在工作"，
进度由调用方自己维护（`setStatusMessage()` 就是给它用的）。

## 应用内通知

外壳自带一个 toast 栈，`showToast()` 开箱可用：

```cpp
shell.showToast(tr("文档已保存"), QxApp::QxToast::Success);

// 需要自己控制位置或同屏上限时，取 manager
QxApp::QxToastManager *toasts = shell.toastManager();
toasts->setPosition(QxApp::QxToastManager::BottomRight);
```

toast 是外壳的**子控件**（不是顶层窗口），因此不置顶、不抢焦点，且在 offscreen 与
WebAssembly 下行为一致。完整说明见 [组件全览](components.md)。

## 布局持久化

```cpp
// 由外壳持有 QxSettings（默认按应用名创建）
QxCore::QxSettings *settings = shell.settings();

// 把窗口几何、停靠布局与当前页写进配置
shell.saveLayout();

// 读回配置。缺失或过期的值会被跳过，因此全新安装时调用是安全的
shell.restoreLayout();
```

配置位于组 `ui`：

| 键 | 内容 |
| :--- | :--- |
| `windowGeometry` | 窗口几何 |
| `windowState` | 主窗口状态（工具栏、状态栏等） |
| `dockState` | 停靠区布局 |
| `currentPage` | 当前页的 id |

`setAutoSaveLayout(true)`（默认）会让 `closeEvent()` 自动调用 `saveLayout()`。

> **注意**：`setSettings()` **故意不自动恢复布局**，这一点与 `QxThemeManager` 不同。
> 布局引用了具体的页面与停靠面板，只有在它们都创建之后才能应用，
> 因此时机由调用方决定 —— 也就是上面的"先建页面和面板，再 `restoreLayout()`"。

## 导航轨

`QxAppShell::navigationBar()` 返回外壳持有的 @ref QxApp::QxNavigationBar 导轨，
一般不需要直接操作；但它可以单独使用：一个竖排的互斥可勾选条目轨（`QButtonGroup`）。

```cpp
QxNavigationBar *rail = new QxNavigationBar;
const int index = rail->addItem(icon, tr("Home"));
rail->insertItem(0, otherIcon, tr("Start"));
rail->setItemEnabled(index, false);

connect(rail, &QxNavigationBar::currentChanged, this, [](int i) { /* ... */ });
```

- **导轨自身不带策略**：添加第一个条目不会自动选中它，只有调用方或点击才会移动选中项。
- 条目按索引寻址，**越界索引一律 no-op** 而不是断言，因此缓存了索引的调用方不会让导轨崩溃。
- 条目插删在选中项之前时，选中索引会随之平移并发信号 —— 把索引当作"导轨中的地址"，
  而不是条目的身份。

## 启动屏

`QxSplashScreen` 在 `QSplashScreen` 的基础上补的是**进度**：

```cpp
QxSplashScreen splash(logo, QStringLiteral("MyApp"), QStringLiteral("3.0"));
splash.show();
splash.step(20, tr("Loading settings..."));
// ... 启动工作 ...
splash.step(100, tr("Ready"));
splash.finish(&mainWindow);
```

- `step(percent, message)` 设置进度与消息后**重绘并处理待办事件**
  （`processEvents(ExcludeUserInputEvents)`）—— 启动工作阻塞主线程，必须自己驱动重绘，
  否则启动屏不会更新。空消息表示保留当前消息。
- 应用名、版本、消息与进度绘制在图片底部，因此一张 logo 就够。
- logo 为空时用一块纯色板兜底，测试与尚无美术资源的应用也能用。

## 启动顺序与高 DPI

Qt 6 默认开启高 DPI；Qt 5 必须在构造 `QApplication` **之前**设置属性。
`demos/qxapp/appshell/main.cpp` 给出了可直接复用的完整启动序列：

```cpp
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("qtcanpool"));
    QCoreApplication::setApplicationName(QStringLiteral("AppShellDemo"));

    QxSplashScreen splash(QPixmap(), QStringLiteral("QtCanpool AppShell"), QStringLiteral("3.0"));
    splash.show();
    splash.step(20, QObject::tr("Loading settings..."));

    MainWindow window;
    splash.step(60, QObject::tr("Building the workspace..."));
    window.restoreLayout();
    splash.step(100, QObject::tr("Ready"));
    splash.finish(&window);

    window.show();
    return app.exec();
```

注意顺序：**先设组织与应用名，再创建外壳**。`QxSettings` 与引擎都按应用名定位配置文件，
顺序反了会写到默认位置。

可运行的完整示例见 `demos/qxapp/appshell`（目标名 `AppShellDemo`）：
三个页面、三个停靠面板、主题切换、跟随系统、布局保存与恢复。
