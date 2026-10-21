# 控件集

本页说清 4.7 新增的两套控件（`qxfluent` / `qxmaterial`）在本仓库的**位置、边界与判据**：

- 它们与 `qxribbon` 是什么关系（**三个选项，不是一套的三种皮肤**）；
- 一件东西该放进 `qxcontrols`（共用）还是各自实现（**判据：答案是否相同**）；
- 两套设计语言的概念在本仓库里**叫什么、落在哪个文件**。

类与函数的完整签名见命名空间页面 @ref QxFluent 与 @ref QxMaterial；能力范围与最小用法见
[组件全览](components.md)。

---

## 一、三个选项，不是一套的三种皮肤

`qxfluent`、`qxmaterial`、`qxribbon` 是**用户三选一的三个选项**，互补而不是重复：

| | `qxribbon` | `qxfluent` | `qxmaterial` |
| :--- | :--- | :--- | :--- |
| 回答的问题 | 命令如何组织（页 / 分组 / 快捷工具栏） | 单个控件在 Fluent 观感下长什么样 | 单个控件在 Material 观感下长什么样 |
| 粒度 | 一整条命令带 | 一个控件 | 一个控件 |
| 与另两者的关系 | 是**命令组织**的一种选择 | 是**观感**的一种选择 | 是**观感**的一种选择 |
| 依赖 | 仅 Qt | `qxcontrols`（+ 可选 `qxwindow`） | `qxcontrols` |

所以「ribbon 里已经有按钮了」**不是** fluent / material 不做按钮的理由：
它们回答的不是同一个问题。两套控件集都**自成体系**——用户选了 material 就只用 material 的控件，
不需要为了一个卡片去引 fluent。

> ⚠️ 三者的观感**互不相干**：一个应用应当**选一套**（外加可选的 ribbon 作命令组织）。
> 混用两套控件集没有禁止，但也没有为它做过任何对齐工作。

## 二、共用还是各自实现：判据是「答案相同」

两套控件集里有一批**同名**控件（Switch / Slider / Badge / Card / ProgressRing）。
同名**不等于**同一个东西：Fluent 的开关与 Material 的开关**形状、动效、状态色都不同**，
所以它们是**各自实现的两份**，不是同一个类的两种皮肤。

一件东西下沉到 `qxcontrols`，当且仅当**两套控件集对它的答案相同**：

| 机制 | 为什么是共用 | 落在哪 |
| :--- | :--- | :--- |
| 状态机（Rest / Hover / Pressed） | 任何控件都要回答指针，答案一致 | `QxControl` |
| 状态层的覆盖不透明度 | 悬停 / 焦点 / 按下 / 拖动四态，两族都要 | `QxStateLayer` |
| 描边（焦点环） | 画法一致，只是颜色不同 | `QxRingPainter` |
| 投影 | 两族都要，且都按「档位 → 一组数字」取 | `QxShadowPainter` |
| 无障碍桥 | `QAccessible` 的接线与角色枚举与观感无关 | `QxControlAccessible` |
| **圆角档位** | **答案不同**（0/2/4/6/8 对 0/4/8/16/28） | **各自**：`FluentShape` / `MaterialShape` |
| **动效时长与曲线** | **答案不同**（Fluent 一组、Material 另一组，且 Material 另分 `Easing`） | **各自**：`FluentMotion` / `MaterialMotion` |
| **控件的形状与画法** | 这正是两族分歧所在 | **各自** |

一句话：**共用机制，不共用观感**。

唯一的横向例外：`qxfluent` 的窗口材质（`FluentWindowMaterial`）是 `qxwindow` 的 Mica / Acrylic /
Mica Alt，因此那一个文件在 `WITH_QXWINDOW` 打开时才侧向依赖 `qxwindow`；关掉时照样编译，
只是回答「本平台没有这种材质」。这条例外在 `src/libs/qxfluent/CMakeLists.txt` 顶部与
[架构与约定](architecture.md) 的**分层**一节都写明了。

## 三、两套语言在本仓库的映射

| 概念 | Fluent（`qxfluent`） | Material（`qxmaterial`） |
| :--- | :--- | :--- |
| 形状 / 圆角 | `FluentShape::Radius`（0/2/4/6/8/9999） | `MaterialShape::Radius`（0/4/8/16/28/9999） |
| 字级 | `FluentType::Ramp` | `MaterialType::Scale`（Label12 … Display36，另有 strong） |
| 高度 / 投影 | `FluentElevation::Level` | `MaterialElevation::Level`（六档：偏移量 / 模糊 / 透明度各一组） |
| 动效 | `FluentMotion::Duration` | `MaterialMotion::Duration` + `MaterialMotion::Easing` |
| 指针高亮 | `FluentReveal`（指针附近的揭示） | `MaterialRipple`（按下处扩散的涟漪） |
| 开关 / 滑块 | `FluentSwitch` / `FluentSlider` | `MaterialSwitch` / `MaterialSlider`（无刻度） |
| 徽标 / 卡片 | `FluentBadge` / `FluentCard` | `MaterialBadge` / `MaterialCard`（三变体） |
| 进度 | `FluentProgressRing` | `MaterialProgressRing`（不确定态为**伸缩弧**，不是定长旋转弧） |
| 反馈 / 占位 | `FluentInfoBar` / `FluentTeachingTip` | `MaterialSnackbar` / `MaterialSkeleton` |

差异的**判据**（写新控件时照这个判断）：

- **同名控件允许长得不一样**——两族的形状、动效、状态色各自取自本族令牌，不要求对齐。
- **只有「两族答案相同」的机制才下沉**（见上一节的表）。
- **动效一律尊重减弱动效开关**：`QxTheme::isMotionReduced()` 为真时，涟漪不画、
  骨架屏停在呼吸中段、进度环停转——「动」的部分关掉，「在不在、是多少」的部分照旧。

## 四、文件与命名

| | Fluent | Material |
| :--- | :--- | :--- |
| 命名空间 | `QxFluent` | `QxMaterial` |
| 类名 | `Fluent*` | `Material*` |
| 文件名 | `fluent*` | `material*` |
| 私有头 | `fluent*_p.h` | `material*_p.h` |

（4.7 §七.9 已定；与既有库的 `qx*` 前缀是两回事——那七个库的名字本身就是 `qx` 开头，
新库的类名前缀取**设计语言名**，是为了让「这是哪一族的控件」在调用处一眼可见。）

## 五、词条

- **a11y**：无障碍。每个控件都要给出角色（`accessibleRole()`，默认 `Client`）与状态，
  滑块 / 进度环另实现 `QAccessibleValueInterface`。
- **覆盖色（wash）**：状态层叠在表面之上的那一层，`washColor()` 返回无效色即不叠。
- **chrome**：控件自己画的非内容部分（如分段按钮的分隔线、轻提示的操作区）；
  `chromeRect()` 是它的矩形，命中测试与键盘导航都认它。

---

## 相关页面

| 页面 | 内容 |
| :--- | :--- |
| [组件全览](components.md) | 各库的能力范围、主要类与最小用法 |
| [架构与约定](architecture.md) | 四层分层、命名空间与 d-pointer 约定、新增库的约定 |
| [主题引擎](theming.md) | 令牌与调色板从哪来、主题切换时什么会变 |
