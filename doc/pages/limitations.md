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

---

## LayoutRequest 可能带来多余的 resize 轮次（LayoutRequest can add resize passes）

> **（4.4 D1）已复核并刷新引用**：A 组重构动过 `ribbonbar.cpp` / `ribbonpage.cpp`，本条原先那批
> 行号因此整体错位（20~40 行），现已按当前代码逐条更正；顺手把散在 6 个内容入口里逐字重复的
> `postEvent(this, new QResizeEvent(size(), size()))` 并回 `RibbonBar::resizeRibbon()`（恒等变换）。
> 结论不变：仍维持现状。

**现象**：`RibbonBar` 收到 `QEvent::LayoutRequest` 时会重跑一次 ribbon resize，
所以一次操作可能触发不止一轮 resize。

**成因**：修的是"该更新时没更新"，代价是"不该更新时也可能更新"。
**（4.2）触发来源已经列清**——逐个 `postEvent` 读代码核对，不是事件日志——一共三类：

| # | 来源 | 位置 | 说明 |
| :--- | :--- | :--- | :--- |
| 1 | `RibbonGroup` **主动投递** | `ribbongroup.cpp:785` / `794` / `808` | `ActionAdded` / `ActionChanged` / `ActionRemoved` 三个分支各投一次，投给 `parentWidget()`（即 page）。源码注释写的是"group 的尺寸变了，要让 page 也调整"。**这是本库唯一的 `LayoutRequest` 生产者**，是有意设计 |
| 2 | **Qt 自己投递** | 任意子部件的 `QWidget::updateGeometry()` → 投给它的 parent | `setLayout()`、`QLayout::invalidate()`、`addWidget` / `removeWidget`、`setSizePolicy`、样式与字体变化、子部件 show / hide、最小最大尺寸变化都会走到这里。本库既控制不了，也数不完 |
| 3 | **三个消费者** | `RibbonBar::event`（`ribbonbar.cpp:1834`）、`RibbonPage::event`（`ribbonpage.cpp:757` → `updateItemGeometry()`）、`RibbonMainWindow::eventFilter`（`ribbonmainwindow.cpp:189` → `updateMinimumWidth()`） | 同一份事件有三个接收者。只有 `RibbonBar` 那处会"多跑一轮 resize"，而它**带 `m_layoutRequest` 闸门**（读在 `ribbonbar.cpp:1847`，只在 corner widget 收到 `ShowToParent` / `MouseButtonRelease` 时于 `:1867` 置位），所以多花的轮次被限制在那一种情形里

**为什么没继续做**：这是性能问题不是正确性问题，没有观察到的故障，也没有可测量的回归基线
（离屏截图测试测的不是这个）。

**要改变它需要什么**：触发条件已列清（上表）。除上表第 1 类之外，本库自己的投递点只剩两处：
`RibbonBar::resizeRibbon()` 的**一处**排队投递（唯一的排队点；6 个内容入口 +
`setQuickAccessBarPosition` / `setTabBarPosition` 都经它请求），以及 **2 处**同步
`sendEvent(this, &resizeEvent)`（`ribbonbar.cpp:1546` / `1581`），外加 `ribbongallery.cpp:184` 的同类投递。
要判断的是"能不能用 `LayoutRequest` 替掉其中某几个"，以及"替掉之后 resize 轮次是变多还是变少"。
⚠️ 这一步需要一份**真机事件日志**来对照（离屏测的不是这个），而且 `resizeRibbon()` 的延迟正是上面
「RibbonBar 的 resize 是延迟的」那一条的来源——动它要连着那一条一起看。

---

## WpsLiteStyle 下按钮组短暂重叠（WpsLiteStyle button group overlap）

> **（4.4 D1）已复核**：落点 `RibbonAppWindow::updateWindowFlags()` 仍在，成因未变
> （它依赖上面「RibbonBar 的 resize 是延迟的」那一条）。结论不变：仍维持现状。

**现象**：`WpsLiteStyle` 下，右侧按钮组与窗口按钮组会有一瞬间重叠。

**成因**：按钮组先显示，ribbon 后 resize；而 ribbon 的 resize 是延迟的（见第一条），
所以"显示"没有办法等到"resize 完成"。曾经试过先隐藏按钮组、等 resize 完再显示——
按钮组仍然在 resize 完成前就回来了。

**出现位置**：`RibbonAppWindow::updateWindowFlags()`。

**要改变它需要什么**：同样落回"让 ribbon 的 resize 变同步"。这是外观问题、瞬时的、不丢状态，
所以排在正确性问题之后。

---

## 定制数据没有草稿态（customization has no draft state）

> **（4.4 D1）已复核**：`apply()` 的两处说明（代码里的 `NOTE` 与本条）逐句核对一致；
> 它要变就是 API 层面的决定（加草稿态或摘掉 `apply()`），而 4.4 承诺不破坏 API。
> 结论不变：仍维持现状。

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

---

> 下面五条来自 **A1**（`doc/design/4.2-TASKS.md`）：`qxwindow` 里从上游继承下来的 8 处
> `TODO`/`FIXME` 注释（连 `### FIXME FIXME FIXME` 这种写法都是上游的），4.1 曾把它们列为非目标，
> 于是长期"没有归宿"。4.2 逐条判定：**两处是死代码、直接删掉**（第一条），
> **其余四处改写成 `NOTE:` 并落到本页**。它们描述的都是 Windows 原生窗口路径，离屏测试覆盖不到。
>
> **（4.3 A1）这五条 + 本页 `qxwindow` 的其余三条又复核了一遍**，逐条结论见各自条目的
> "（4.3 A1）"标记。摘要：**八条里七条维持现状**（各自"要改变它需要什么"都要求真机或 WM 侧证据，
> 离屏环境给不了，硬修等于改一个看不见的行为），**一条删掉了恒空的分支**
> （"边框接管光标"那条，`dontOverrideCursor`）——因为那一条的删除是**可证明的恒等变换**，
> 不属于"改看不见的行为"。

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

## 可缩放窗口的边框一定会接管光标（hit testing always overrides the border cursor）

**现象**：只要窗口**可缩放**，鼠标进入边框区域时一定会变成缩放光标；调用方**没有任何办法**
让命中测试返回普通光标（例如希望边框只用于阴影、拖动只走标题栏）。

**成因**：命中测试里曾有一个 `dontOverrideCursor` 标志，字面意思就是"不要覆盖光标"，
但它**没有任何来源**——`WindowContextWin::windowProc()` 里写下 `bool dontOverrideCursor = false;`
之后，全文件再没给它赋过值。于是它的四处使用全部退化为 `isFixedSize`
（`(isFixedSize || dontOverrideCursor) ? … : …` 等价于 `isFixedSize ? … : …`），
"不要覆盖光标"那条分支**永远走不到**。这是上游遗留的空壳。

**（4.3 A1）已删掉这个空壳**：既然它恒为 `false`，四处使用就等价于 `isFixedSize`，
所以删除是**可证明的恒等变换**——不是"改一个看不见的行为"，因此不需要真机验证
（这与 4.2 删掉那两段 `#if 0` 是同一类操作）。行为逐字未变：边框仍然接管缩放光标。
留着的坏处是**一个恒假的分支配一个像开关的名字**，下一个人会以为这里能配置。

**为什么保持现状**：调用方**仍然没有任何办法**让命中测试返回普通光标
（例如希望边框只用于阴影、拖动只走标题栏）——这正是本条限制本身。4.3 删的是**误导**，
不是**限制**。

**要改变它需要什么**：给 `WindowAgent` 加一个"边框不接管光标"的开关并接到命中测试里——
那是**新的公开 API**，归 A5（公开 API 面评估），不在本条里顺手做。

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

## 自定义 QToolButton 应用按钮按下会下沉（custom QToolButton application button sinks on press）

> **（4.4 D1）已复核**：demo 里的引用（`demos/qxapp/ribbondemo` 的 `QXRIBBON_TEST_CUSTOMIZE_APPBTN`
> 分支）仍在，成因未变——是 `QToolButton` 的标准按下视觉，库不该替调用方的控件定样式。
> 结论不变：仍维持现状。

**现象**：用调用方自己提供的 `QToolButton` 当 ribbon 应用按钮时，单击（按下）按钮内容会下沉，
与 ribbon 自带的 `RibbonApplicationButton` 观感不同。

**成因**：这是 `QToolButton` 的**标准按下视觉**（按下时下压内边距），不是 bug。`RibbonApplicationButton`
是 qxribbon 自己的类，ribbon 样式表覆盖了它的按下观感；但样式表**覆盖不到调用方提供的任意 `QToolButton`**，
所以这个标准视觉就露出来了。`demos/qxapp/ribbondemo` 在 `QXRIBBON_TEST_CUSTOMIZE_APPBTN` 开时
就用 `QToolButton` 当应用按钮，因此看得到这个下沉（`mainwindow.cpp` 里用本页英文名引用了本条）。

**为什么保持现状**：要消除，调用方得自己给 `QToolButton` 设样式（例如 `padding: 0` 或针对 `:pressed`
的重写）。这是"用自己的控件"必然带来的代价，库不该替调用方的控件定样式。

**要改变它需要什么**：调用方给应用按钮 `QToolButton` 设一条针对 `:pressed` 的样式；或改用
`RibbonApplicationButton`（它已经没有这个下沉）。

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

## 本版的验证边界：CI 不覆盖 Qt 6.12（no CI coverage for Qt 6.12）

> 这一条与上面那些**不是同一类**：上面记的是"知道但不打算改的**行为**"，
> 这一条记的是**证据的边界**——本版**在哪些东西上没有证据**。

**事实**：`ci.yml` 的矩阵是四条腿——`ubuntu` / `windows` / `macos` 各一条 **Qt 6.8.1**，
加一条 `ubuntu` 的 **Qt 5.15.2**；`pages.yml` 里另有一条 wasm 腿，钉 **6.8.3**。
**Qt 6.12 LTS 已发布（2026-09-30）**，而 4.3 **没有**为它加腿（决策 **K31**：基线不动）。

**影响**：本版**没有**在 Qt 6.12 上构建或测试过的证据。6.8 → 6.12 之间的任何变化都落在盲区里，
已知的一类例子是与 `QStyleHints::colorScheme()` 相关的行为；这类变化**不会**被 CI 发现，
只能等有人在 6.12 上真的用一次。
（这也意味着：**本页其它条目里凡是"Qt 6 上如何"的描述，证据都只到 6.8**。）

**为什么保持现状**：作者拍板的**已知取舍**，不是遗漏。加一条腿要付构建时间与缓存维护的成本，
而 6.12 发布才一个月；新 LTS 的验证排到下个版本。

**要改变它需要什么**：在 `.github/workflows/ci.yml` 的 `matrix.include` 里加一条
`{ os: ..., qt: '6.12.x' }`，并确认 wasm 腿与缓存键不与之耦合；跑绿之后**本条就可以删掉**。

> 相关：Qt 5 那条腿的去向（以及你该提前准备什么）见
> [迁移指南](migration.md) 的「5.0.0 将移除 Qt 5 支持」一节。
