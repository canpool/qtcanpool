# 已知限制

这一页记的是 **QtCanpool 里那些"知道但不打算改"的行为**：它们不是待办事项，而是有理由的现状。
每一条都写清了现象、成因、为什么保持现状，以及要改变它需要付出什么。

判断标准来自决策 **K21**（见 `doc/design/4.1-TASKS.md`）：代码里出现的每一处 `TODO` / `FIXME`
只有三条出路——**修好**（附回归测试）、**写进这一页并改成 `NOTE:`**、或者**删掉并说明为何不是问题**。
"留着等以后"不在其中。所以这一页上有的条目，说明那件事已经被判断过了。

> 这里**不是缺陷列表**。真正的缺陷在 issue 里，修不动的、或者修了不划算的才在这里。

---

## RibbonBar 的 resize 是延迟的

**现象**：切换 Ribbon 样式时，新的几何到下个事件循环轮次才生效。

**成因**：`RibbonBar::resizeRibbon()` 用 `QApplication::postEvent()` 投递一个 `QResizeEvent`，
而不是直接调 `resizeEvent()` 里的那段逻辑。直接调会坏的：WPS 样式下无边框/有边框切换之后，
`m_applicationButton.isVisible()` 会突然返回 `false`，尺寸计算随之出错。

**代价**：延迟是可以观察到的。WPS 样式 + 最小模式下切到 OFFICE 样式时，
`setFixedHeight(d->m_tabBar->geometry().bottom())` 会在 tab bar 被 resize 之前读到旧的 bottom，
高度于是停在 WPS 的值，tab bar 所在的水平区域显示不出来。
`RibbonBar::setRibbonStyle()` 里为此**直接**调 `d->resizeRibbon()` 绕过队列——那里需要立刻拿到新几何。

**要改变它需要什么**：把这条链路变成"同步且不重入"，即让 resize 立刻完成而不会在样式切换的中途
读到半旧的状态。目前没有已知的写法能同时满足这两点；改的时候注意 `RibbonBar::event()` 里的
`QEvent::LayoutRequest` 分支（见下一条），它与这里是同一个问题的两面。

---

## LayoutRequest 可能带来多余的 resize 轮次

**现象**：`RibbonBar` 收到 `QEvent::LayoutRequest` 时会重跑一次 ribbon resize，
所以一次操作可能触发不止一轮 resize。

**成因**：修的是"该更新时没更新"，代价是"不该更新时也可能更新"。而且**哪些情况会产生 LayoutRequest
从来没有被列清过**，也没有梳理过哪些 `postEvent()` / `sendEvent()` 可以由它代替。

**为什么没继续做**：这是性能问题不是正确性问题，没有观察到的故障，也没有可测量的回归基线
（离屏截图测试测的不是这个）。目前的抑制手段是 `m_layoutRequest` 开关——
只有 corner widget 是 `QMdiArea` 时才置位，所以多花的轮次被限制在那一种情形里。

**要改变它需要什么**：先把 LayoutRequest 的触发条件列清（这需要在一台真机上打事件日志），
再决定能不能用它替换掉现有的 `postEvent()` 调用。

---

## WpsLiteStyle 下按钮组短暂重叠

**现象**：`WpsLiteStyle` 下，右侧按钮组与窗口按钮组会有一瞬间重叠。

**成因**：按钮组先显示，ribbon 后 resize；而 ribbon 的 resize 是延迟的（见第一条），
所以"显示"没有办法等到"resize 完成"。曾经试过先隐藏按钮组、等 resize 完再显示——
按钮组仍然在 resize 完成前就回来了。

**出现位置**：`RibbonWindow::updateWindowFlags()` 与 `RibbonAppWindow::updateWindowFlags()`
（同一段逻辑，两处都有）。

**要改变它需要什么**：同样落回"让 ribbon 的 resize 变同步"。这是外观问题、瞬时的、不丢状态，
所以排在正确性问题之后。

---

## 定制数据没有草稿态

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

## Linux 上的无边框窗口

**现象**：Linux 上用无边框窗口，最大化之后拖标题栏还原不正常。

**成因**（两处都是**平台行为**，不是本库能修的 bug）：

| 环境 | 现象 |
| :--- | :--- |
| Ubuntu 16.04 LTS / Unity 7.4.0 | 最大化后拖标题栏拿到的 `normalGeometry()` 不是原始尺寸，而是"无边框最大化后的尺寸"；只有抓住 Unity 自己画的那条标题栏才能拿到真值 |
| GNOME | `normalGeometry()` 是对的，但会一闪而过、随后尺寸又变回最大化；而无边框窗口无法被移到显示区之外，于是**根本移不动** |

**现在的态度**：Windows 与 macOS 的无边框照旧；Linux 上它属于"需要 WM 配合"的实验性路径。
是否把 Linux 的默认改成系统边框，是决策 **K23**（见 `doc/design/4.1-TASKS.md`），
**待作者拍板**——因为它改的是默认观感，不是修 bug。

---

## Qt 6.5 及以上的无边框依赖一个动态属性

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
