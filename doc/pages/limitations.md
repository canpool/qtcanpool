# 已知限制

这一页记的是 **QtCanpool 里那些"知道但不打算改"的行为**：它们不是待办事项，而是有理由的现状。
每一条都写清了现象、成因、为什么保持现状，以及要改变它需要付出什么。

判断标准来自决策 **K21**（见 `doc/design/4.1-TASKS.md`）：代码里出现的每一处 `TODO` / `FIXME`
只有三条出路——**修好**（附回归测试）、**写进这一页并改成 `NOTE:`**、或者**删掉并说明为何不是问题**。
"留着等以后"不在其中。所以这一页上有的条目，说明那件事已经被判断过了。

> 这里**不是缺陷列表**。真正的缺陷在 issue 里，修不动的、或者修了不划算的才在这里。

**条目名**：标题后面圆括号里的英文名，就是源码注释里引用本页时用的那个字符串
（形如 `// See doc/pages/limitations.md, "..."`）。两边必须**逐字一致**，
否则在页面上 `grep` 不到——改标题时顺手改引用。

---

## RibbonBar 的 resize 是延迟的（RibbonBar resize is deferred）

> **（4.4 D1）已复核**：A 组重构改过 `ribbonbar.cpp`，但没有改到本条的行为。复核时发现原文
> 看不出两个**同名** `resizeRibbon()` 各扮演什么角色（公开那个只排队、私有那个干活），已补写清楚；
> 结论不变：仍维持现状。

**现象**：切换 Ribbon 样式时，新的几何到下个事件循环轮次才生效。

**成因**：重名的一对函数是理解这条的入口。`RibbonBar::resizeRibbon()`（**公开**）只做一件事——
用 `QApplication::postEvent()` 投递一个 `QResizeEvent`；真正干活的 `RibbonBarPrivate::resizeRibbon()`
是**私有**的，由 `resizeEvent()` 到达之后调用。之所以"排队"而不是"直接干"：直接在 `resizeEvent()`
里做会坏的——WPS 样式下无边框/有边框切换之后，`m_applicationButton.isVisible()` 会突然返回 `false`，
尺寸计算随之出错。

**代价**：延迟是可以观察到的。WPS 样式 + 最小模式下切到 OFFICE 样式时，
`setFixedHeight(d->m_tabBar->geometry().bottom())` 会在 tab bar 被 resize 之前读到旧的 bottom，
高度于是停在 WPS 的值，tab bar 所在的水平区域显示不出来。
`RibbonBar::setRibbonStyle()` 里为此**直接**调 `d->resizeRibbon()`——那是**私有**那个，同步干活、
绕过队列，因为那里需要立刻拿到新几何。

**排队只有一个入口**：所有会改变 bar 内容的入口都经 `RibbonBar::resizeRibbon()` 请求重排
（6 个内容入口 + `setQuickAccessBarPosition` / `setTabBarPosition`），所以上面那个"为什么排队"
的理由只有一处住所。同文件另有两处**同步** `sendEvent(this, &resizeEvent)`，那是"必须立刻重排"
的相反意图，不在本条内。

**要改变它需要什么**：把这条链路变成"同步且不重入"，即让 resize 立刻完成而不会在样式切换的中途
读到半旧的状态。目前没有已知的写法能同时满足这两点；改的时候注意 `RibbonBar::event()` 里的
`QEvent::LayoutRequest` 分支（见下一条），它与这里是同一个问题的两面。

**（4.5 F2）这条延迟曾经有一处会漏到屏幕上，已修，并因此多了一个同步出口**：
`RibbonBar::resizeRibbonNow()`（`RibbonMainWindow` 有对应转发）做的是**同一份活、立刻做完**。
它**不是**把延迟改成同步，而是「**调用方自己声明"我这里必须现在拿到新几何"**」的写法——
按上面的理由，**样式切换那条路径仍必须用排队版**，所以两个入口并存、由调用方选。

要它的那条路径是 `QxApp::RibbonAppWindow::updateWindowFlags()`——唯一一个「**窗口已经在屏幕上、
却还要改右侧按钮组与窗口按钮组**」的入口（基类实现是 `setWindowFlags()` + `show()`，
窗口在那一刻就重新显示了）。它此前请求的是**排队**版，于是那一轮事件循环里屏幕上留的是**旧几何**：
用户看到的那一帧就是两个组按老边界画出来的**重叠**（`TabBarInlineStyle` 下最明显）。
改用 `resizeRibbonNow()` 后，`updateWindowFlags()` **返回的那一刻**两个组已经是新几何。
⚠️ 由此得到的**使用约束**写进了 `resizeRibbonNow()` 的**头文件注释**（`src/libs/qxribbon/ribbonbar.h`）：
它只能由"**调用方已经在看**"的路径使用，**样式切换那条路径必须继续排队**——延迟的理由正是不让它中招。
回归用例 `tst_RibbonAppWindow::flagsChangeLaysTheGroupsOutImmediately()`（离屏成立，进 18/18 门禁），
探针是把它换回排队版即红。

---

## LayoutRequest 可能带来多余的 resize 轮次（LayoutRequest can add resize passes）

> **（4.4 D1）已复核并刷新引用**：A 组重构动过 `ribbonbar.cpp` / `ribbonpage.cpp`，本条原先那批
> 行号因此整体错位（20~40 行），现已按当前代码逐条更正；顺手把散在 6 个内容入口里逐字重复的
> `postEvent(this, new QResizeEvent(size(), size()))` 并回 `RibbonBar::resizeRibbon()`（恒等变换）。
> 结论不变：仍维持现状。
>
> **（4.6 C1d）闸门改写，行号再次失效**：原先写的「`m_layoutRequest` 闸门，只在 corner widget 收到
> `ShowToParent` / `MouseButtonRelease` 时置位」**从来没生效过**——那个分支在 `RibbonBar::eventFilter()`
> 里，而全仓库没有把 bar 装成角控件的事件过滤器，`m_layoutRequest` 恒为 `false`，`LayoutRequest`
> 分支等于死代码（这也是「corner widget 从来没被验证过」的另一面）。`m_layoutRequest` 与那个分支一并
> 删除，闸门换成一条可测的条件（`RibbonBarPrivate::cornerWidgetsNeedLayout()`）：**角控件不在布局给
> 它的位置上**——宽度 ≠ 它的 `sizeHint()` 宽度，**或**纵向范围 ≠ 它所在的那一行带。
>
> 两半各有来由。宽度那一半：角控件是这块 bar 里唯一「尺寸不由本类决定」的子控件，布局给它的宽度就是
> 它当时要的宽度，所以宽度一变就说明摆过的位置过期了。行带那一半：`QMenuBar` **自己也会摆角控件**
> （Qt 5/6 的 `QMenuBarPrivate::updateGeometries()`：左角在整条 bar 的高度里垂直居中、右角贴
> `PM_MenuBarVMargin` 顶对齐），那是菜单栏的摆法而不是 ribbon 的；它给的宽度又正好是 `sizeHint()`
> 宽度，所以只看宽度的话，MDI 子窗口最大化那一幕会被判成「位置没问题」，而实际图标被摆到了标题行
> 下方。`LayoutRequest`（Qt 摆完自己会 `updateGeometry()`）是这一刻唯一的信号，也正是本类唯一收得到
> 的那次。多花的轮次仍被限制在「角控件与布局的约定不符」这一种情形里，比原来的条件更窄。

**现象**：`RibbonBar` 收到 `QEvent::LayoutRequest` 时会重跑一次 ribbon resize，
所以一次操作可能触发不止一轮 resize。

**成因**：修的是"该更新时没更新"，代价是"不该更新时也可能更新"。
**（4.2）触发来源已经列清**——逐个 `postEvent` 读代码核对，不是事件日志——一共三类：

| # | 来源 | 位置 | 说明 |
| :--- | :--- | :--- | :--- |
| 1 | `RibbonGroup` **主动投递** | `ribbongroup.cpp:785` / `794` / `808` | `ActionAdded` / `ActionChanged` / `ActionRemoved` 三个分支各投一次，投给 `parentWidget()`（即 page）。源码注释写的是"group 的尺寸变了，要让 page 也调整"。**这是本库唯一的 `LayoutRequest` 生产者**，是有意设计 |
| 2 | **Qt 自己投递** | 任意子部件的 `QWidget::updateGeometry()` → 投给它的 parent | `setLayout()`、`QLayout::invalidate()`、`addWidget` / `removeWidget`、`setSizePolicy`、样式与字体变化、子部件 show / hide、最小最大尺寸变化都会走到这里。本库既控制不了，也数不完 |
| 3 | **三个消费者** | `RibbonBar::event`、`RibbonPage::event` → `updateItemGeometry()`、`RibbonMainWindow::eventFilter` → `updateMinimumWidth()` | 同一份事件有三个接收者。只有 `RibbonBar` 那处会"多跑一轮 resize"，而它**带闸门**（4.6 C1d 起是 `cornerWidgetsNeedLayout()`：某个角控件不在布局给它的位置上——宽度或所在行带不符——才重排），所以多花的轮次被限制在「角控件与布局的约定不符」这一种情形里 |

**为什么没继续做**：这是性能问题不是正确性问题，没有观察到的故障，也没有可测量的回归基线
（离屏截图测试测的不是这个）。

**要改变它需要什么**：触发条件已列清（上表）。除上表第 1 类之外，本库自己的投递点只剩两处：
`RibbonBar::resizeRibbon()` 的**一处**排队投递（唯一的排队点；内容入口与
`setQuickAccessBarPosition` / `setTabBarPosition` 都经它请求），以及 `RibbonBar::resizeRibbonNow()` 的
同步 `sendEvent(this, &resizeEvent)`（`setMinimized()` / `showMinimumButton()` 走它）。
（`RibbonGallery` 原先也有一处排队投递，4.5 把选项板分组改为自绘之后它已不存在。）
要判断的是"能不能用 `LayoutRequest` 替掉其中某几个"，以及"替掉之后 resize 轮次是变多还是变少"。
⚠️ 这一步需要一份**真机事件日志**来对照（离屏测的不是这个），而且 `resizeRibbon()` 的延迟正是上面
「RibbonBar 的 resize 是延迟的」那一条的来源——动它要连着那一条一起看。

---

## 定制数据没有草稿态（customization has no draft state）

> **（4.4 D1）已复核**：`apply()` 的两处说明（代码里的 `NOTE` 与本条）逐句核对一致；
> 它要变就是 API 层面的决定（加草稿态或摘掉 `apply()`），而 4.4 承诺不破坏 API。
> 结论不变：仍维持现状。
> **（4.5 F5）复核**：4.5 虽允许破坏 API，但修复是"功能"不是"修 bug"——界面路径本就安全
> （无 Apply 按钮，OK 同时应用+保存），只有代码直调 `apply()` 才走到失败序列，而归调用方负责。
> 两条出路（加草稿态 / 摘掉公开 `apply()`）都是设计决策，本版不单独为它开。无代码改动。

**现象**：`RibbonCustomizeDialog::apply()` 会把改动立刻写进 ribbon，但**不写文件**。
"应用后直接关掉对话框、不保存"会丢掉这次改动；更麻烦的是：
一次会话里新建页并应用，下次会话里删掉它再应用，会产生一条指向**已不存在对象**的删除记录，
此后保存就把这条删除写进文件。

**为什么现在是这样**：对话框**没有 Apply 按钮**——一次定制以 OK 结束，OK 会同时做"应用"和"保存"，
所以上面的序列从界面走不到，只能由代码直接调 `apply()` 触发。
`RibbonCustomizeWidget`（真正持有临时数据的那个类）**没有导出**，
它的生命周期归对话框所有：取消就等于销毁它，数据随之废弃，因此不需要再加一层缓冲。

**要改变它需要什么**：给 `apply()` 加一层草稿态（写入前后可回滚），或者把 `apply()` 从公开接口上拿掉。
两者都是 API 层面的决定，不属于"修一下"。

---

## Linux 上的无边框窗口（frameless windows on Linux）

> **（4.3 A1）维持现状**：触发条件仍窄（"最大化后再拖"一个动作），而翻默认会让**所有** Linux 应用
> 多出一条系统标题栏，与 ribbon 标题行并存（K23 已定不改）。真要修的是"最大化→拖拽还原"这条路径本身，
> 那是 WM 相关代码，不是默认值问题。

**现象**：Linux 上用无边框窗口，最大化之后拖标题栏还原不正常。

**成因**（两处都是**平台行为**，不是本库能修的 bug）：

| 环境 | 现象 |
| :--- | :--- |
| Ubuntu 16.04 LTS / Unity 7.4.0 | 最大化后拖标题栏拿到的 `normalGeometry()` 不是原始尺寸，而是"无边框最大化后的尺寸"；只有抓住 Unity 自己画的那条标题栏才能拿到真值 |
| GNOME | `normalGeometry()` 是对的，但会一闪而过、随后尺寸又变回最大化；而无边框窗口无法被移到显示区之外，于是**根本移不动** |

**现在的态度**：Windows 与 macOS 的无边框照旧；Linux 上它属于"需要 WM 配合"的实验性路径，
**并且继续是 Linux 的默认**——决策 **K23**（见 `doc/design/4.1-TASKS.md`）问的就是要不要把
Linux 的默认改成系统边框，结论是**不改**。

为什么不改：上面两条的触发条件都很窄——都是"**最大化之后再去拖**"这一个动作，
普通拖动不受影响。而把 Linux 默认改成系统边框，换来的是一件事更宽的影响面：
`RibbonBar` 仍然会画自己那条标题行，于是窗口上会同时出现系统标题栏和 ribbon 标题行，
**所有** Linux 应用的外观都会变，而且是变差。用一个窄问题的修法去换一个宽问题，方向不对。

**要改变它需要什么**：真正该修的是"最大化 → 拖拽还原"这条路径本身（它读的 `normalGeometry()`
在 Unity 上不对、在 GNOME 上会闪），那是平台相关的窗口管理代码，不是把默认值翻过来。
真要翻，位置已经不在 `qxribbon` 里了：4.3 起这个库不再自带无边框实现（`RibbonWindow` 与它的
后端 `FramelessHelper` 一并删掉，见[迁移指南](migration.md)），Linux 上要无边框只能走 `qxapp` 的
`RibbonAppWindow`。

---

## Qt 6.5 及以上的无边框依赖一个动态属性（frameless windows on Qt 6.5 and later）

> **（4.3 A1）维持现状，缺的是证据不是代码**：这条记的是"证据的边界"——
> 离屏截图测不到窗口边距，只能在装了 Qt 6.5.3 的真机上跑一次真实窗口才能收口（issue `#I8SPBC` / `#I8SPC8`）。
> 目前没有已知故障，不构成改动的理由。

**现象**：Qt 6.0 起，自定义窗口边距改走 Qt 的**私有平台类**（`QWindowsWindow::setCustomMargins()`）；
到 Qt 6.5，那批平台私有头已经不再随 Qt 一起安装（`QtGui/<ver>/QtGui/private/` 里没有 `qwindowswindow_p.h`），
所以那段代码被 `QT_VERSION < 6.5.0` 挡住了。

**现状**：Qt ≥ 6.5 时，`setInternalWindowFrameMargins()` 只设置动态属性 `_q_windowsCustomMargins`，
实际读取它的是 Qt 自己的 Windows 平台插件。这一点是**验证过的**：Qt 6.8.3 的
`plugins/platforms/qwindows.dll` 里含有 `_q_windowsCustomMargins` 这个字符串（`Qt6Gui.dll` 里没有），
即插件自己会认这个属性。

**还没做的验证**：在一台装有 **Qt 6.5.3** 的机器上跑一次真实窗口（issue `#I8SPBC` / `#I8SPC8`
记录的就是这个待验证点）。离屏截图测试测不到窗口边距，所以这件事只能在真机上看。
目前没有已知故障，这一条记的是"证据的边界"，不是"已知的坏行为"。

---

## Windows + Qt 6 上，无边框窗口不能"启动即最大化"（a frameless window on Windows, Qt 6, maximised on first show）

> **（4.3 A1）维持现状**：成因在 Qt 6 的 Windows 平台插件里（纯 Qt 程序即可复现），本库改不了。
> **（4.3 A10）本条的性质变了**：`FramelessHelper` 与 `RibbonWindow` 已在 4.3 删掉，
> 本库**不再有会踩到它的无边框路径**——只剩 `RibbonAppWindow`，它走 qxwindow 的原生路径，
> 实测不受影响。本条之所以留着，是因为成因在 Qt 自己的平台插件里：**任何**用
> `Qt::FramelessWindowHint` 的程序都会踩，而且它正是本库要走原生路径的理由。
> **（4.3 A3）不再只靠人看**：本条现在是**可自动判定的**，见下面「怎么自己验」。

**现象**：Windows 上无边框窗口如果**第一次显示就是最大化**——在事件循环之前调 `showMaximized()`，
最典型的是在构造函数里——之后拖标题栏"还原"只会让窗口**平移**、尺寸仍是满屏；再拖边框，要么
拖不动，要么在最大化的尺寸上来回弹。Qt 6.8.3 复现，Qt 5.15.2 正常。

**成因**：这是 **Qt 6 在 Windows 平台插件里的回归，与本库无关**。一个最小的纯 Qt 程序就能复现
（一个 `QWidget` 加 `Qt::FramelessWindowHint`，不含 qtcanpool）：调 `showNormal()` 之后，Qt 自己
记账的 `windowState()` 变成 `WindowNoState`，而原生窗口的 `WS_MAXIMIZE` 还在
（`GetWindowLongPtrW(GWL_STYLE)` 仍带这一位）；同一次调用还把 `normalGeometry()` 改写成
**最大化之后的矩形**，所以"还原"出来仍然是满屏。有边框窗口没有这个现象，Qt 5 与 Qt 6 都正常。

触发要三个条件同时成立：**无边框 + 第一次 `show` 就是最大化 + Windows**。

**避开方式**：Windows + Qt 6 上要"启动即最大化"，二选一——① 先 `show()`，再 `showMaximized()`；
② 让第一次 `show()` 用普通尺寸，再 `QTimer::singleShot(0, &w, &QWidget::showMaximized)`。
本库现有的无边框窗口（`QxApp::RibbonAppWindow`）不受影响。

**为什么保持现状**：这是 Qt 平台插件里的状态同步问题，本库无从改起。本库**曾经**有一条会中招的
路径：`RibbonWindow` 走 Qt 自带的 `Qt::FramelessWindowHint`（Qt 侧模拟）；4.2 按 K27 把它标弃用，
4.3 按 K26 把缓冲走完——连同它的后端 `FramelessHelper` 一起删掉（见[迁移指南](migration.md)）。
`RibbonAppWindow` 用的是 qxwindow 的 `WindowAgentWidget`：保留 `WS_THICKFRAME | WS_CAPTION`、
用 `WM_NCCALCSIZE` 把整窗变成客户区，最大化与还原仍归系统管，所以**实测不受影响**
（`demos/qxapp/ribbondemo` 正是启动即最大化，拖标题栏能正确还原、拖边框逐像素跟手）。

**要改变它需要什么**：不在本库——要修的是 Qt 的 Windows 平台插件，应用层只能按上面的"避开方式"
绕开。本库当年放弃的那条修法是：在 `FramelessHelper` 里自己记住"最大化之前的几何"，并在还原时
显式让系统退出最大化（`ShowWindow(SW_RESTORE)`），也就是把那条模拟路径改成 `WindowAgentWidget`
那样的原生做法；它要在一套已被取代的模拟路径上重建原生状态同步，所以 4.2 的 K27 没选它。

**怎么自己验（4.3 A3）**：`tests/qxwindow/auto/tst_qxwindowframeless.cpp` 有两条用例，把本条
从"只能靠人看"变成了"能自动判定"：

- `nativeWindowIsNotFrameless()`——**本库自己的窗口不带 `Qt::FramelessWindowHint`**。三个触发条件里
  只有这一条是本库能控制的，所以这一条才是本库真正守得住的回归；它离屏也成立，CI 每腿都跑。
  （判据用窗口标志而不是平台名：CI 的每条腿都在 `offscreen` 下，平台名答不出后端是谁。）
- `restoreReturnsThePreMaximisedGeometry()`——**不含 qtcanpool 的纯 Qt 探针**：无边框窗口
  `showMaximized()` 首秀，再 `showNormal()`，断言几何回到最大化之前的尺寸。它在
  **Qt 6 + Windows** 上记为 `QEXPECT_FAIL`（**实测确实复现**：Qt 6.8.3 / Windows 11 命中 XFAIL，
  Qt 5.15.2 通过），所以**今天它是绿的，Qt 修好那天会变成 XPASS 报出来**——这正是"可检测化"要的东西。
  它需要真的窗口管理器，离屏下 `QSKIP`（离屏没有 WM，窗口也没有可与 Qt 对账的原生状态）。

⇒ 也就是说：本条现在**不必等人发现**，Qt 一侧一变、或者本库哪天又接回无边框路径，都有断言会喊。
⚠️ CI 目前**跑不到探针本体**（每条腿都是 `offscreen`，探针会 skip）；能跑到的是第一条。

**（4.5 F1）另一半也钉住了**：4.3 A3 只钉住了本库能控制的那一半，**"本库不受影响"此前只是人测过的文字**。
F1 先在**真实显示**上复核了它，结论是**本条不改库**：

| 同一序列（首秀即最大化 → `showNormal()`），Qt 6.8.3 / Windows 11 / 真实显示 | 纯 Qt 无边框窗口 | 本库 agent 的窗口 |
| :--- | :--- | :--- |
| `IsZoomed()` | **1**（仍是最大化，即本条现象） | 0 |
| `GWL_STYLE & WS_MAXIMIZE` | **仍在** | 已清除 |
| 几何 | 不是最大化前的尺寸 | 回到最大化前的尺寸 |

成因那条路径要一个**主动向 Qt 要 `Qt::FramelessWindowHint`** 的窗口，而本库的窗口不是：
原生 agent 保留 `WS_THICKFRAME`/`WS_CAPTION`、靠答 `WM_NCCALCSIZE` 去掉边框，最大化与还原始终归系统管。
⇒ `windowcontext_win.cpp` **没有一处可修**；为"本库窗口到不了的状态"补防御代码是**死代码**，不是修复。

于是 F1 补的是**证据**：`tests/qxwindow/native/tst_qxwindownative.cpp` 新增
`maximisedFirstShowStillRestores()`——**首秀即最大化**（这是触发条件，所以它写不成"先 build 再 show"的用例），
`showNormal()` 之后断言上表那三项都归位。它在**唯一有窗口管理器可问的那套件**里跑。
⚠️ 与探针本体一样**不在门禁**：CI 每条腿都是 `offscreen`（该用例会 skip），
`tst_qxwindow_native` 本来就是一个非门禁步骤；本机真实显示下 Qt 6.8.3 与 Qt 5.15.2 均通过。
**反证**：把该用例指向一个纯 Qt 无边框窗口，第一条断言即红（`IsZoomed != 0`）——
它咬的正是本条现象，不是走过场。

---

> 下面四条（原五条）来自 **A1**（`doc/design/4.2-TASKS.md`）：`qxwindow` 里从上游继承下来的 8 处
> `TODO`/`FIXME` 注释（连 `### FIXME FIXME FIXME` 这种写法都是上游的），4.1 曾把它们列为非目标，
> 于是长期"没有归宿"。4.2 逐条判定：**两处是死代码、直接删掉**（第一条），
> **其余四处改写成 `NOTE:` 并落到本页**。它们描述的都是 Windows 原生窗口路径，离屏测试覆盖不到。
>
> **（4.3 A1）这五条 + 本页 `qxwindow` 的其余三条又复核了一遍**，逐条结论见各自条目的
> "（4.3 A1）"标记。摘要：**八条里七条维持现状**（各自"要改变它需要什么"都要求真机或 WM 侧证据，
> 离屏环境给不了，硬修等于改一个看不见的行为），**一条删掉了恒空的分支**
> （**原**"边框接管光标"那条，`dontOverrideCursor`）——因为那一条的删除是**可证明的恒等变换**，
> 不属于"改看不见的行为"。那条记的限制本身已在 4.6 D1 解决（`WindowAgent::setBorderOwnsCursor()`），
> 整条随之从本页移除。

---

## 非客户区的触摸与指针输入不做处理（non-client touch and pointer input）

> **（4.3 A1）维持现状**：把 `WM_NCPOINTER*` 接进来需要另一套状态机，是**加功能**而非修问题；
> 还要一台真机触摸设备才能验证，本机没有。

**现象**：在无边框窗口的非客户区（`RibbonBar` 自己画的标题行、边框）上，触摸与笔输入的非客户
消息（`WM_NCPOINTERUPDATE` / `WM_NCPOINTERDOWN` / `WM_NCPOINTERUP`）**完全不处理**：既不会像
鼠标那样被重定向到客户区，也不会被 Windows 自己的非客户区逻辑接走。

**成因**：`WindowContextWin` 里有一张"把非客户鼠标消息重定向成客户鼠标消息"的表
（`emulateClientAreaMessage()`），触摸/笔消息从来没有进过这张表。上游留下的是两段 `#if 0` 的空壳
（三个 `case` 全是 `break`），**与"不处理"完全等价**，4.2 把它删掉了。

**为什么保持现状**：无边框窗口的交互整体建立在鼠标消息语义上（拖动、命中测试、窗口按钮的
hover / 按下都依赖 `WM_NC*` 鼠标消息）；把 pointer 消息接进来需要另一套状态机，属于**加功能**
而不是修问题。

**要改变它需要什么**：把 `WM_NCPOINTER*` 接到与对应鼠标消息同一个分支（上游那两段 `#if 0`
就是这个意思），并补一台**真机触摸设备**上的验证——这条路径离屏测试测不到。

---

## 命中测试里对 WM_NCMOUSEMOVE 调用了 DefWindowProc（DefWindowProc on WM_NCMOUSEMOVE during hit testing）

> **（4.3 A1）维持现状**：删掉这一句仍是"改一个看不见的行为"——窗口按钮的 hover / 按下 / 拖动
> 离屏环境全测不到，而本页下一条已经证明：即使像它这样**可证明恒等**的改动也能顺手删，
> 这一条**不可证明**（"重复跑一遍 DefWindowProc"是否真无副作用没人回答过），故不动。

**现象**：鼠标停在窗口按钮（chrome button）上移动时，本库处理完这条非客户消息之后，还会把它
原样交给 `DefWindowProcW()`，并把它的返回值当作自己的返回值交给 Windows。

**成因**：从上游继承而来，原注释用三行 `### FIXME FIXME FIXME` 写着
"Calling DefWindowProc() here is really dangerous, investigate how to avoid doing this"。
可以确定的是：这条消息**已经被本库重定向成客户区的鼠标移动**，所以这次调用是在"已经处理过一次"
的基础上，再跑一遍 Windows 自己的非客户区默认处理，而其结果被逐字返回。

**为什么保持现状**：**没有人回答过"能不能避免"**，而这条路径**没有回归测试**——窗口按钮的
hover、按下、拖动在离屏测试里都测不到。在没有真机验证的前提下删掉这一句，属于"改一个看不见的
行为"，风险大于收益。按 K21，原注释从"待办（`FIXME`）"降级为"已知（`NOTE`）"。

**要改变它需要什么**：在真机上分别记录"调用 / 不调用"两版时窗口按钮的 hover 与拖动行为
（事件日志），确认这一句是否多余；确认多余之后再删，并且要给窗口按钮补一条能跑在离屏环境里的
回归用例——否则下一次改动仍然无法验证。

---

## WM_WINDOWPOSCHANGING 的 SWP_NOCOPYBITS 绕法（WM_WINDOWPOSCHANGING with SWP_NOCOPYBITS）

> **（4.3 A1）维持现状**：触发动作与修法都是上游记录过的，唯一缺的是**解释**，而解释需要
> WM 侧资料或抓帧观察。绕法已生效、现象已消失，这条**没有待办**。

**现象**：在 Windows 设置里切换"在标题栏和窗口边框上显示主题色"、或者调用 `DrawMenuBar()` 时，
Windows 会发出一条 flags 恰为 `0x37` 的 `WM_WINDOWPOSCHANGING`。**不处理它**，客户区会整体向
左平移；不重绘的话看着很不对劲。

**成因**：**不明**。本库的处理是往 `flags` 里补一个 `SWP_NOCOPYBITS`，补上之后现象消失。
至于"为什么会发生"以及"为什么这样能解决"，上游原注释的原话是
"I don't know what caused the problem, or why this would solve it"。

**为什么保持现状**：触发动作与修法都是上游**记录过的**（`kBadWindowPosFlag` 就是那五个 flag 的
或），唯一缺的是**解释**。按 K21，这种"修好了但说不清原因"的条目写进本页、注释降级为 `NOTE`，
而不是留一条 `FIXME` 在源码里当待办——它**没有待办**，绕法已经生效。

**要改变它需要什么**：搞清 `SWP_NOCOPYBITS` 在这里究竟避免了哪一次位块传送
（大概需要 Windows 窗口管理器侧的资料，或者 `Spy++` / 抓帧级别的观察）。在弄清之前**不动它**。

---

## 最大化或全屏时不能拖动窗口（no move while maximised）

> **（4.3 A1）维持现状**：正经做法"先还原、再跟随"是**新行为**，且需要"还原后几何"这个
> 当前在这一层拿不到的量。本页"启动即最大化"那条要一起验证，两者都需要真机。
> **（4.5 F4）复核**：维持现状，理由同上。修复属新行为而非修 bug，且还原几何在此层仍拿不到；
> 上游即"干脆不动"，本库沿用。无代码改动。

**现象**：`WindowMoveManipulator`（这一层用来实现"拖动窗口"的小对象）在窗口处于最大化或全屏时
**直接放弃**：拖标题栏没有任何反应。

**成因**：这里拿不到窗口"还原后的尺寸"。它的做法是记住按下时的窗口位置与鼠标位置，之后按位移
`setPosition()`；而最大化状态下窗口的"正常几何"不是它当前的几何，照算会得到一个错误的位置。
上游的处理就是干脆不动，本库沿用。

**为什么保持现状**：正经做法是"先还原、再跟随"（Windows 原生标题栏就是这样），那是**新行为**
而不是修 bug；而且它需要"还原后的几何"这个当前在这一层拿不到的量。

**要改变它需要什么**：在 `WindowMoveManipulator` 里保存"最大化之前的几何"，在第一次移动时先
退出最大化、再按位移走。注意这与本页"Windows + Qt 6 上，无边框窗口不能'启动即最大化'"那条相邻
——那边是 Qt 平台插件的状态不同步，这里是这一层没有还原几何，两者要放在一起验证。

---

## qxdock 停靠目标只有九个固定区域（qxdock docks to nine fixed areas）

> **（4.5 F6 / G2）盘点结论**：qxdock 在 G2 限制盘点里**唯一一条**实打实的设计性限制。
> 其余被怀疑的项（浮动窗 snapping 精度、`saveState`/`restoreState` 状态、隐藏标题栏几何）
> 复核后均不成立：`saveState` 把树里每个面板都写进 XML（关闭/隐藏的 widget 仍在树中、照写），
> 浮动窗几何也单独存（`DockContainer::saveState` 的 `Geometry` 段）；frameless 宿主由 C8 的
> `tst_DockWindow::framelessHost` 钉住。所以 qxdock 的限制页目前**只有这一条**。

**现象**：拖动一个 dock widget 时，屏幕上只出现**九宫格**式的投放预览（中心 + 上下左右 + 四角），
松手后 widget 停靠到对应的那一块；没有"贴着别的 dock 边缘出现参考线""吸附到任意像素位置"
这类细粒度引导。

**成因**：停靠判定完全走 `DockOverlay` 的九个固定 `DockWidgetArea`，预览与落点都是这九个离散区域，
库里没有任何"吸附网格 / 参考线 / 像素级 snap"的实现（全仓 `grep snap|grid|magnetic` 零命中，
除 `QGridLayout` 与 overlay 内部九格布局）。这是 ADS 一脉 docking 库的通用取舍：离散区域够用、
且视觉不可验证，所以没有去做连续吸附。

**为什么保持现状**：九区域覆盖绝大多数停靠意图；连续吸附是纯观感改进，本库无金标、不可视验证，
收益不足以抵消"改了没人能审"的风险。

**要改变它需要什么**：在 `DockOverlay` 之上加一层参考线 + 像素级吸附判定，并配套一个能断言
"吸附到 X 像素"的视觉/几何测试（离屏不足以审，需要真机或图片比对）。

---

## qx_add_resources 不对文件路径做 XML 转义（qx_add_resources does not escape file paths）

**现象**：用 `qx_add_resources()`（以及本库其它生成 `.qrc` 的宏）时，资源文件的路径会被**原样**写进
生成的 `.qrc`；若路径里含有 `&` `<` `>` `"`，生成的 `.qrc` 不是合法的 XML。

**成因**：生成 `.qrc` 时只做了字符串拼接，没有对路径做 XML 转义。绝大多数资源路径是相对或绝对路径、
不含这些字符，所以平时碰不到；但 `&` 在 Windows 文件名里合法、`<` `>` `"` 在 Linux 文件名里合法，
理论上可以出现（`cmake/QtCanpoolAPI.cmake` 里用本页英文名引用了本条）。

**为什么保持现状**：现有资源路径都不含这些字符，没有已知触发；做转义要改构建 API 的行为，且要确保
不破坏现有资源。属于"知道但不打算现在改"。

**要改变它需要什么**：在生成 `.qrc` 时对路径做 XML 转义（转 `&amp;` `&lt;` `&gt;` `&quot;`）。
改完要在 CI 里用一条含特殊字符的资源路径验证生成的 `.qrc` 可解析。

---

## macOS 只构建 qxwindow 与 qxdock（macOS builds qxwindow and qxdock only）

> **（4.5）2026-10-17 定**：CI 矩阵里 macOS 那条腿原先是「整棵树 + 跑测试」。
> 作者要求收窄为「只构建涉及 macOS 的目标」，理由、边界与所失覆盖核对于下。
> ⚠️ 定稿前踩过一次：只按「谁有原生后端」筛，得出「只有 `qxwindow`」的结论，
> 漏掉了 `qxdock` 的三处 `#ifdef Q_OS_MACOS`。**判据是「哪些文件里有 `#ifdef Q_OS_MAC`」，
> 不是「谁实现了平台后端」**——后者会漏掉在别人家代码里做平台分支的库。

**事实**：`ci.yml` 的 `build` 矩阵里 `{ os: macos-latest, qt: '6.8.1' }` 那条腿，
**只构建 `qxwindow` 与 `qxdock` 两个目标**，且**不跑测试**——`WITH_DEMOS` / `WITH_EXAMPLES` /
`WITH_TESTS` 三条全 `OFF`，构建命令末尾按平台拼 `--target qxwindow qxdock`，
`Test` 步骤对这条腿跳过。实测（本机 MinGW 同参配置 + 构建）：产物只有
`libqxwindow.dll` 与 `libqxdock.dll`，未拖进第三个库。

**理由**：七个库里**只有两个**含有 macOS 专属代码（判据：`grep -rl Q_OS_MAC src/libs`）——
- **`qxwindow`**（8 个文件含 `Q_OS_MAC` 分支）：`windowagentbase.cpp` /
  `windowagentwidget.{cpp,h,_p.h}` / `windowcontext.{cpp,_p.h}` / `windowcontext_qt.cpp` /
  `windowsystem_p.h`；另有**两个整文件的 Mac 实现**（整文件只在 macOS 编，内部无需分支）：
  `windowcontext_cocoa.mm`（全树唯一的 Objective-C++ 文件）与 `windowagentwidget_mac.cpp`。
- **`qxdock`**（3 个文件）：`dockfloatingcontainer.cpp:605` 的 `activateWindow()`
  （`updateDropOverlays()` 之后）、`:782` 起**整个 `event()` 重载**（约 45 行，
  处理 `NonClientAreaMouseButtonPress` 与「拖拽把最大化窗口拖下来」的冲突）、
  `dockfloatingcontainer.h:83` 的对应声明，以及 `qxdock_global.h` 的 `QX_DOCK_X11`
  平台卫哨（`!defined(Q_OS_MACOS)`，与 Windows / wasm 并列排除 X11）。

其余五个库——`qxcore` / `qxtheme` / `qxribbon` / `qxplugin` / `qxapp`——**一个 `Q_OS_MAC` 都没有**，
在 macOS 上编的是与 Linux、Windows **完全相同的源文件**，而那两边已有两条腿覆盖。
在最慢的 runner 上重编它们是**纯墙钟**，换不来任何门禁覆盖。

**影响**：那条腿如今只回答一个问题——**两个平台分支还能不能编过**。
它**不再**回答「整棵树在 macOS 上能不能编过」「测试在 macOS 上过不过」。

⚠️ **所失的覆盖**（有意接受，据下）：
- `tst_qxwindow` / `tst_qxdock` 在 macOS 上不再跑。两个套件本身是可移植的，
  在另外两条腿上照跑。`tests/qxwindow/auto/tst_qxwindowgeometry.cpp` 里唯一的
  `#ifdef Q_OS_MACOS`（`:297`）是个 **`QSKIP`**——它在 macOS 上把该用例整个**跳过**，
  而非做 macOS 专属断言，所以在这里跑它本来也拿不到 macOS 上的检查结果。
- `demos/qxwindow/mainwindow.cpp` 的 `#ifndef Q_OS_MAC` 分支（系统按钮那几段）不在 macOS 腿上编。
  ⚠️ 这**不是本次收窄造成的**：macOS 腿的 `WITH_DEMOS=OFF` 在此之前就已如此，
  那些分支此前也没有任何腿在编。且 demo 不被任何测试执行、其 macOS 分支里的也只是通用 Qt 代码。

**为什么保持现状**：省掉的是**已被另外两条腿证明过**的那部分重复编译，
保留的是**只有这一条腿能证明**的那部分。两者按「文件里有没有 `#ifdef Q_OS_MAC`」划开，
不按「库看起来重不重要」划。

**要改变它需要什么**：把三个 `WITH_*` 改回 `ON` 并去掉 `Test` 步骤的 `if`，即本条记录之前的状态。
若只想再加回某个库，往 `--target` 后追加即可——但请先用
`grep -rl Q_OS_MAC src/libs/<lib>` 确认它确实含有 macOS 分支，否则是白付墙钟。

---

## RibbonMdiArea 的首个文档要点过别的标签页才会激活（the first MDI document needs a tab visit to activate）

> **（4.5 B5）盘点结论**：这是 `QMdiArea` 自身在 tabbed + document 模式下的行为，
> `RibbonMdiArea` 无法从公共 API 绕过，因此记录在此并写进类的文档注释。

**现象**：`RibbonMdiArea` 以标签模式打开**第一个**文档后，该文档的标签处于选中外观，
但 `activeSubWindow()` 仍为 null——此时 `setActiveSubWindow()` 对它是 **no-op**
（标签已是当前项，`currentChanged` 不会再来），标签栏也不容许撤选（`setCurrentIndex(-1)` 被忽略），
视图模式往返同样无效。用户直接点这个已选中的标签不会有任何反应，该文档登记的
`RibbonPage` 也要等访问过另一个标签并点回来才会出现。

**成因**：Qt 的 tabbed 视图在插入首个标签时自动选中它，而 `QMdiArea` 的活动窗口是
`null`；它的激活完全由标签栏的 `currentChanged` 驱动，对"已经是当前项"的标签无从触发。
实测（Qt 5.15.2 / 6.8.3 一致）：先加文档后 `show()` 会激活首个文档；先 `show()` 后加文档
（即应用运行中「新建」的第一个文档）则停留在此状态；存在第二个文档后一切恢复正常。

**为什么保持现状**：没有公共 API 能打破它（撤选、`setActiveSubWindow`、视图模式往返都试过，
均有探针记录）。此状态下 `RibbonMdiArea` 的不变量依然自洽——无活动文档 ⇒ 无登记页在上，
宿主未登记的页（如主页）照常显示，这正是文档类应用启动时的典型观感。

**要改变它需要什么**：绕开 `QMdiArea` 自绘文档标签栏（自绘即引入不可视验证的视觉语义，
与本库既有取舍冲突），或等 Qt 修掉"首个标签自动选中但不激活"这一不对称。

---

## Qt 5 上 MDI 子窗口最大化后系统按钮的图标看不到（the MDI child's window buttons on Qt 5）

> **（4.6）新增记录**：这条和 ribbon 的 `cornerWidget` 布局是同一处——`QMdiArea` 把最大化
> 子窗口的窗口图标与最小/还原/关闭按钮装到宿主窗口的菜单栏上（`QMenuBar::setCornerWidget`），
> ribbon 窗口里那个菜单栏就是 `RibbonBar`，于是由
> `RibbonBarPrivate::resizeInTabBarBelowStyle` / `resizeInTabBarInlineStyle` 摆放。
> 布局这边是对的（几何有测试 `tst_RibbonBar::maximizedMdiChildUsesTheCorners`），坏在
> Qt 自己那三个按钮的画法上。

**现象**：Qt 5 下，主题样式表生效时，MDI 子窗口最大化后出现在标签行右端的三个系统按钮
**字形基本看不到**——只在按钮底边留一点残影；100% 缩放下图标整体偏下、贴着底边，
高分屏（125% / 200%）下因为偏差同样被放大，看上去就是"图标没显示"。Qt 6 不受影响。

**成因**：这三个按钮是 Qt 的私有部件 `QMdi::ControllerWidget`，它**自己不算尺寸**，
`sizeHint()` 按菜单栏的字体给出（实测 9pt 字体 ⇒ 67x23），而平台样式按
`QStyle::PM_TitleBarHeight` 画标题栏按钮，`QStyleSheetStyle` 报告的该值是 **30**
（高分屏下按倍数放大），按钮字形因此画在这个高度矩形的底部，超出部件矩形被裁掉。
两者口径不同是 Qt 5 的问题：`QMdi::ControllerWidget` 用字体算高度、样式用像素度量画。
plain `QMainWindow` + 同一个样式表可以复现（与 ribbon 无关），ribbon 只是给了它一个
22~23 像素高的矩形。探针记录：部件 67x23、`PM_TitleBarHeight=30`、偏移随缩放倍数增大。

**为什么保持现状**：`QMdi::ControllerWidget` 是 Qt 私有类，画法是样式的事，`qxribbon`
只能决定给它的矩形。给得再高也没用（探针：把它拉到 30 高之后反而整个不画），
要"修"只能自绘这三个按钮——那是引入不可视验证的视觉语义，还要与 Qt 的 MDI 行为脱钩。

**要改变它需要什么**：Qt 侧统一 `QMdi::ControllerWidget` 的 `sizeHint()` 与
`PM_TitleBarHeight` 的口径（Qt 6 已经不再画这套标题栏按钮，所以只在 5.15 上体现）；
或在宿主里替换 `QMdiArea` 的那套角标（自绘 + 失去 Qt 的窗口菜单联动）。

---

## 主题的样式表和 dock 的样式表都是「应用级」，只能留一份（the theme's sheet and the dock's sheet are both application level）

> **（4.6 B4）新增记录**：给 `dockdemo` 接主题引擎时撞见的，是 4.5 把 dock 外观交还给应用之后
> 留下的接缝：`qxdock::dockStyleSheet()` 要应用自己 `setStyleSheet()`，
> 而主题引擎 `QxThemeManager::apply()` 也在同一个位置 `setStyleSheet()`。

**现象**：一个既用主题引擎、又用 dock 的应用，两张应用级样式表**互相覆盖**——后装的赢。
先装 dock 表、再切主题，dock 表被主题表顶掉（dock 回到未装饰的样子）；先切主题再装 dock 表，
主题表被顶掉（整个界面退回平台外观）。只有一条路走得通：**把两张拼成一条字符串装上去**，
并且在每次主题变更后重拼一次。

**成因**：Qt 的应用级样式表是**一个字符串**（`QApplication::styleSheet()`），不是列表；
`qxribbon` 与 `qxdock` 都在 4.5 退回到「外观由应用装」（B12 / C9），两个库于是都要求应用
在同一个位置装自己那张。widget 级样式表能避开，但优先级更高，会把应用主题里对这些控件的规则
整个盖掉——那正是 4.5 拆掉 widget 级 sheet 要解决的事，不能走回头路。

**现状**：`demos/qxapp/dockdemo` 自己拼：`engine->styleSheet(engine->theme()) + m_dockSheet`，
并监听 `themeChanged` 重拼。拼的顺序是**主题在前、dock 在后**：dock 表按类名描述 dock 自己的
控件，不会盖掉主题对同一控件的规则；反过来则会让 dock 的纯色背景压过主题的背景。

**要改变它需要什么**：让应用级样式表可以由几段组成——引擎在装主题表时把附加段接在后面
（`QxThemeManager` 加一个「附加样式表」入口，`dockStyleSheet()` 的文本交给它而不是应用自己拼）。
那是**加法**（不删不改现有签名），但要想定下来，得先回答一个问题：附加段由谁拥有、
主题表为空（"no skin" 主题）时还算不算、以及两段都写到同一个控件时谁赢（现在靠拼接顺序决定，
这个约定得写进文档）。

---

## offscreen 下有几件事是看不见的（what an offscreen test cannot see）

> **（4.7 I1）新增记录**：4.7 把「场景测试」当成一等公民之后攒下来的一页。
> 这一条的读者是**写测试的人**，不是用库的人：它说的是在 `QT_QPA_PLATFORM=offscreen`
> 下，哪些断言**看起来在测、其实没测到**，以及哪些根本测不了。

**现象**：下面每一条都是一次「测试绿了但没测到东西」的实况，按撞到的顺序记：

1. **`grab()` 自带不透明背景**：`render()` 带 `DrawWindowBackground`，所以每个 widget 抓下来
   都是 100% filled——**「没画」读不出来**（dump 里每个 widget 都写 100% 就是这个）。
   能用的只有**主色及其占比**；要测「画了多少」得**跟页面色比**。
2. **平台度量随 style 变**：滚动条在 fusion 16 / windowsvista 17 / windows11 12（暗色恒 16），
   菜单栏与工具栏由平台样式画 ⇒ **整窗截图比较**在换平台或换主题后不可靠。
   dock 的截图比较已改成「两图各按自己 `--inspect` 读到的 dock 矩形裁剪，只比共同高度那一段」。
3. **指针要真的挪**：`RibbonButton::drawButton()` 这类代码**不看 widget 的 hover 记账**，
   而是问 `QCursor::pos()` 在不在 rect 里。⇒ ① 测 hover 必须 `QTest::mouseMove` 真移指针
   （`sendEvent(Enter)` 不够）；② **「静止」那张基准图必须先把指针挪开**——它停在上一个用例
   留下的位置，常常正好在被测控件里，于是「rest == 已 hover」，断言全假绿。
4. **延迟删除不算消失**：`deleteLater()` 的东西在 `processEvents()` **之后还在树里**
   （dock 取消 pin 时 side tab 就是在容器的析构里才 delete 的）。
   ⇒ `Probe::settle()` 除了 `processEvents(AllEvents)` 还补了
   `sendPostedEvents(nullptr, QEvent::DeferredDelete)`。
5. **没 show 的 widget 没有几何**：布局没跑 ⇒ 位置全是 (0,0)、splitter `sizes()` 为空。
   读几何之前必须 `show()` + settle 一拍（`apply()` 之后同理）。
6. **拖放不能用 `QDropEvent`**：Qt 只接受真拖拽投递的 drop，`sendEvent()` 的那个会被
   **静默丢弃**，`dropEvent()` 根本不进。⇒ 测「落下来之后发生什么」走
   `DockWindow::handleDockWidgetDrop()`（它是 `dropEvent()` 的全部本体）。
7. **半透明 tool window 抓出来是空的**；`(not painted)` / `transparent` 的 widget
   无法参与「规则有没有生效」这类断言（矩阵测试里 `SpacerWidget`、两个 `DockOverlay` 就跳过了）。
8. **字体**：Qt 不再随包发字体（offscreen 下每笔都刷 `Cannot find font directory` 的警告），
   所以**与字号相关的绝对值不可测**。dock / ribbon 的 chrome 高度是**样式表里写死的数字**
   （实测 17pt 字体下 tab 栏仍是 24）——要动 chrome 得动样式表，不是动字体。
9. **内部类（没有导出宏）无法独立链接测试**：`_p.h` 里的类、`QSplitter` 之外的度量算术、
   `RibbonButtonPrivate::calcIconAndTextRect()` 这类几何，都只能**透过公开门**
   或**在测试里子类化暴露 protected 成员**来间接测。给它们加 `Q_AUTOTEST_EXPORT`
   需要库与测试**两端**都按 `QT_BUILD_INTERNAL` 构建，属独立工程。

**成因**：offscreen 平台没有窗口系统——没有真实指针、没有真实窗口、没有字体服务；
而「画了什么」这件事本来只有抓图一条路，抓图又自带背景。

**现状**：能绕的都绕进测试基建 `tests/common/tst_probe.{h,cpp}`（`Probe` 命名空间）：
`settle()`（含延迟删除）、`painted()` / `glyph()`（数主色与占比，而不是数像素）、
`image()` / `differingRect()` / `differingPixels()`（按区域比两张图）、
`revealCollapsed()`（展开 auto hide 容器）、`fillsItself()`（跳过 hidden 的 splitter 格）。
不能绕的（1、2、6、7、8、9）写在这里。

**要改变它需要什么**：1 需要自己 render 到透明底（`grab()` 没有这个开关）；
2 需要按平台/Qt 大版本分基线（截图基线已经是按 `CMAKE_SYSTEM_NAME`/`Qt<N>` 分的）；
3–5 是测试写法问题，不属库；6 只能走 `handleDockWidgetDrop()`；
9 需要 `QT_BUILD_INTERNAL` 的双端接线。
