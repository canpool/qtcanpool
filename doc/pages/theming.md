# 主题引擎

`qxtheme` 提供进程级的主题引擎，即 @ref QxTheme::QxThemeManager

## 它解决什么问题

一个应用的外观是两件事：一块调色板，和一张样式表。只做其中一件，表面就对不齐 ——
菜单、原生对话框、工具提示、禁用态文字这些样式表覆盖不到的地方仍停在平台默认色，
和一张深色样式表明显不搭。`QxThemeManager` 把两者作为一件事一起应用，并补上持久化与系统跟随。

引擎**不带任何主题**。主题是应用的：应用把「一个 id、一个显示名、是否深色、一张样式表」
注册进来，引擎只负责记住选择、下发、持久化与跟随系统。这样 `qxtheme` 不必知道
`qxribbon` 存在，也不必知道一套外观叫什么名字。

> 4.5 之前 `qxribbon` 自带一套 `RibbonTheme`（widget 级样式表加载器，8 套内置 CSS），
> 与引擎互不相干。两套并存时 widget 级那张会赢：应用选了深色，Ribbon 区仍是浅的。
> 4.5 起 `RibbonTheme` 已删除，8 套 CSS 移到仓库的 `res/themes/`，由应用自行注册。

## 注册主题

```cpp
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

using namespace QxTheme;

QxThemeInfo dark(QStringLiteral("soft-dark"), QStringLiteral("Fluent Dark"), true);
dark.styleSheet = readFile(QStringLiteral(":/themes/soft_dark.css"));

QxThemeManager::instance()->registerTheme(dark);
```

一条注册有八个字段，**只有 `id` 是必填的**（它是写进配置文件的那个值），其余留空各有默认：

| 字段 | 默认 | 说明 |
| :--- | :--- | :--- |
| `id` | — | 稳定标识；唯一必填字段 |
| `name` | 空 | 给人看的名字，设置对话框用 |
| `dark` | `false` | 决定引擎用 `lightPalette()` 还是 `darkPalette()` |
| `styleSheet` | 空 | 空是**合法注册**：清掉应用样式表、只留调色板，即「不换肤」也是一个可选项 |
| `accent` | 无效色 | 强调色；引擎把它装成 `QPalette::Highlight`（见下一节） |
| `onAccent` | 无效色 | 压在强调色上的那个颜色（`QPalette::HighlightedText`） |
| `density` | `Comfortable` | 主题在位时自绘控件按哪一档间距画（见「密度与动效」） |
| `motionScale` | `1.0` | 全场动效时长的倍率，由 `apply()` 装（见「密度与动效」） |

`res/themes.qrc` 把仓库 `res/themes/` 下的样式表挂到 `:/themes/` 上，需要它们的应用
在 CMake 里把这份 qrc 加进自己的源文件即可。仓库自带的主题与约定 id 如下（每族明暗各一套，
生成方式见 `tools/themes/README.md`）：

| 配置 id | 显示名 | 深色 |
| :--- | :--- | :---: |
| `classic-light` | Classic Light | |
| `classic-dark` | Classic Dark | ✓ |
| `soft-light` | Soft Light | |
| `soft-dark` | Soft Dark | ✓ |
| `flat-light` | Flat Light | |
| `flat-dark` | Flat Dark | ✓ |
| `slate-light` | Slate Light | |
| `slate-dark` | Slate Dark | ✓ |
| `accent-light` | Accent Light | |
| `accent-dark` | Accent Dark | ✓ |

家族名描述的是**界面骨架的形状**，不是谁的产品：`classic` 是抬高纸牌的灰条，
`soft` 是把明暗层次反过来的圆角开口，`flat` 是只用强调色下划线标选中的单层面，
`slate` 是深灰条上靠顶部强调线分选中的界面，`accent` 直接用强调色实填选中标签。

**配置 id 是对外契约**：它被写进配置文件，改动需要配合配置迁移。
4.5 换掉了整套预置主题（旧 id `light-office2013` / `dark-wps` 等已不存在），
因此从 4.4 升上来的用户存下的 id 会取不到——引擎对未注册 id 保持原样并告警一次。
想保留老选择的应用，可以自己把旧 id 重新注册到某套新样式表上。

查询已注册的主题：

| 函数 | 说明 |
| :--- | :--- |
| @ref QxTheme::QxThemeManager::registerTheme | 注册（同 id 覆盖）。注册到「当前选中」的那个 id 时立即下发，但不广播 |
| @ref QxTheme::QxThemeManager::unregisterTheme | 按 id 注销 |
| @ref QxTheme::QxThemeManager::hasTheme | 该 id 是否已注册 |
| @ref QxTheme::QxThemeManager::themeIds | 全部 id，按注册顺序 |
| @ref QxTheme::QxThemeManager::themeInfo | 取某个 id 的注册信息（含显示名、是否深色） |

## 基本用法

在 `QApplication` 构造完成之后取得单例即可，选择会被持久化，下次启动自动恢复：

```cpp
QxThemeManager *themes = QxThemeManager::instance();
themes->setTheme(QStringLiteral("soft-dark"));
```

`setTheme()` 会立即应用并发出 `themeChanged()`；`apply()` 用于把当前主题重新装一遍
（比如在外部改动了样式表之后）。
选中一个**未注册**的 id 时引擎保持原样并告警一次，而不是装一张空样式表 ——
清空会抹掉调用方自己设置的样式。

## 运行时切换为什么不需要遍历控件

应用主题做三件事：色方案（见下一节）、`QApplication::setPalette()` 与
`QApplication::setStyleSheet()`。
Qt 随后会向所有部件派发 `QEvent::StyleChange`，而 `qxribbon` 与 `qxdock` 早已有为该事件
准备的既有处理（例如 `RibbonBar` 收到后重排整条 Ribbon —— 标签条的宽度是把它自己的标签
加起来算出来的，换一张表标签变宽了就得重算；`DockTab` 重刷样式）。
因此引擎不需要知道有哪些窗口存在。

⚠️ **这三步的顺序是色方案先、调色板次之、样式表最后，不能反过来**。应用级样式表会让 Qt 给被它命中的
每个部件换上**该部件自己的**调色板，此后 `QApplication::setPalette()` 就再也到不了它们；
样式表最后落下，装它的这一步同时就是让这些部件按新调色板重新 polish 的时刻。
先表后板在「亮 → 暗」时往往看不出来（样式表本身把颜色写成了暗色），在「暗 → 亮」时就会
把编辑器这类部件的底色留在上一套主题里。`QxThemeManager` 一直按这个顺序装；自己手写
`setPalette` + `setStyleSheet` 的应用照抄这个顺序即可。

也正因为是 **应用级**下发：把样式表贴在窗口自己身上会盖掉应用的那张，
这正是 4.5 之前 Ribbon 区不跟随主题的原因。窗口不带主题，应用下发的才到得了 Ribbon。

## 强调色是一个对

主题决定「这个应用用什么颜色强调」——选中页签的填充、dock 焦点标签的填充、指针下窗口按钮的填充，
以及 `QPalette::Highlight`。它**不能由样式表发布**：`Highlight` 是控件问「这个应用强调什么」时
读的那个值，而画在强调面上的字形与图标读的是 `HighlightedText`。

`QxThemeInfo::accent` 与 `onAccent` 就是这两个值，**要成对看待**：

- 两个都不给：引擎用自带高亮，与「没有注册任何主题」时同一个答案。
- 只给 `accent`：引擎从它算一个 `onAccent`（按亮度取黑或白）。
- **两个都给（推荐）**：样式表本来就是照这对颜色画的。让引擎算第二色，等于回答一个
  主题已经回答过的问题，而且答案不同——饱和蓝比它看起来更暗，按亮度阈值会配黑字，
  而写表的人是配白字的。

预置主题的这对颜色写在 `res/themes/accents.json`（一个配置 id 一条），注册时照抄：

```json
"accent-light": { "accent": "#0078d4", "on-accent": "#ffffff" }
```

用 `qxapp` 的 `registerBundledThemes()` 一键注册时，这对颜色会从 `accents.json` 自动填进每个
`QxThemeInfo`——只有自己逐条注册时才需要照抄。

## 密度与动效

这两件事由主题声明、由**自绘控件**去读，都不改样式表——表里的间距与字号是生成期写死的，
换不到运行时的选择。

**密度**：`QxThemeInfo::density`（`Comfortable` / `Compact`）。自绘控件排每一处间隙与内边距时
问它，而不是把数字写进自己的绘制里。预置五族都是 `Comfortable`（表说的就是这一档）；
`Compact` 留给「这套主题本来就该更紧」的注册项。⚠️ 它只影响**代码里画的**那一半，
表里的 padding 不跟着变——要改表得改生成器里那族的档。

**动效**：`qxthememotion.h` 给一套词汇，按「这段动效是干什么的」命名，不按「多长」命名：

| 档位 | 用在哪 | 时长 / 曲线 |
| :--- | :--- | :--- |
| `motionFast` | 用户没要求看的：悬停、状态层 | 120ms，`OutQuad` |
| `motionNormal` | 有距离要走的：展开、淡入、滑动 | 240ms，`OutCubic` |
| `motionEmphasized` | 用户主动要的入场：浮层打开、对话框抵达 | 360ms，`OutBack` |

控件写 `motionFor(tier)` 取值，把 `duration` 交给计时器、`curve` 交给 `QPropertyAnimation`。
`motionFor()` 自己会乘上 `motionScale()`（来自 `QxThemeInfo::motionScale`，由 `apply()` 安装，
默认 `1.0`），所以换一套「更利落」或「更从容」的主题时，控件不用知道倍率是多少。

**减弱动效**是应用自己的无障碍开关——Widgets 读不到系统的「减少动效」偏好，所以由应用在
它自己的设置页里问用户：

```cpp
themes->setReduceMotion(true);   // 记住，下次启动还是这样
```

它**持久化**在配置的 `ui` 组（`reduceMotion`）。打开后 `motionFor()` 的每一档都回
「时长 0 + 线性」，于是**读到它的动画器自动合规**，不需要每个控件自己记得先判一次开关。
进程级的那份状态由 `QxTheme::isMotionReduced()` / `setMotionReduced()` 读写，管理器构造时
把它对齐到配置里的值。⚠️ 预置五族主题与 `qxribbon` / `qxdock` 不读这套词汇，行为不变。

## 调色板之外：平台色方案

调色板和样式表说的是**应用画什么**；另一半是**平台替应用画什么**——窗口边框、非客户区，
以及 Win11 上 `windows11` 样式自己那套控件。这一半由 `QStyleHints::colorScheme()` 决定，
和调色板**没有关系**：

> Qt 6.7 起，Win11 上的默认样式是 `windows11`。它按**系统**的深浅色从自己的明、暗两张表取色
> （`WINUI3ColorsDark` 那一组），**从不读调色板**；窗口边框也看色方案
> （`QWindowsWindow::setDarkBorder`）。于是在暗色桌面上装一套浅色主题，得到的是浅色的 Ribbon
> 配深色的输入框、深色的滚动条和深色的标题栏——而且没有任何调色板条目能改到它们。

主题本来就回答了这个问题（注册项的 `dark`），引擎只是把同一句话也说给平台听：

```cpp
QxTheme::applyColorScheme(true);      // 这套主题按暗色画
QxTheme::followSystemColorScheme();   // 或者：交还给系统
```

`QxThemeManager` 自己会调：`setTheme()` 时按主题的 `dark` 下发；**跟随系统时不覆盖**，而是交还
平台——跟随靠的正是 `colorSchemeChanged`，钉死色方案等于把这条信号掐掉。

- 只有 Qt 6.8+ 有 `QStyleHints::setColorScheme()`。6.8 以下（以及不接受这个请求的平台）这两个
  函数是空实现，那时唯一的口子是整条换掉样式：`windowsvista` 按调色板画，`ribbondemo` 就是这么
  兼容 6.7 的。
- 手写 `setPalette` + `setStyleSheet` 的应用照上面的顺序加一行即可，不必用引擎。
- 色方案也要在**窗口显示之前**定下来：它是窗口第一次绘制就用到的值，装晚了会先画一帧系统色。

## 跟随系统深浅色

```cpp
themes->setSystemThemePair(QStringLiteral("soft-light"),
                          QStringLiteral("soft-dark"));
themes->setFollowSystemTheme(true);
```

引擎**不自带**默认配对（它不知道应用注册了哪些 id），因此开启跟随之前必须先给出这一对。
开启跟随之后：

- 立即应用与当前系统匹配的主题；
- 之后系统切换时自动跟随（Qt 6.5+ 通过 `QStyleHints::colorSchemeChanged`）；
- **持久化的是"跟随"这个开关本身，而不是它推导出的主题**，因此用户此后仍能显式选一个主题；
- 在不报告系统色方案的平台上，可以手动调用 `refreshSystemTheme()` 重新评估。
- 跟随期间引擎**不钉死色方案**，而是把它交还平台（见上一节）；钉死等于让
  `colorSchemeChanged` 不再触发，跟随会停在原地。

## 自定义样式表

样式表是注册信息的一部分，也可以事后替换：

```cpp
// 直接给出内容（传空串可撤销）
themes->setStyleSheet(QStringLiteral("soft-dark"), myCss);

// styleSheet(id) 可以查看 apply() 实际会装载的内容
```

### 应用自己的第二张表

应用级样式表只有一个字符串，所以两张表只能互相顶掉：引擎和应用各自往
`QApplication::setStyleSheet()` 里写，后写的赢，另一张就整张不见了。要表达
「主题那张之外，我还想加几条自己的规则」，用 `setAdditionalStyleSheet()`：

```cpp
themes->setAdditionalStyleSheet(myCss);   // 立刻重新装一遍
```

它拼在**主题表之后**——这个顺序正是它的用途：两边都写的同一个选择器、同特异性时，
后声明的这条决定；主题没提到的选择器照旧落到。三条边界：

- **不持久化**，也**不属于任何主题**：它是应用的文本，什么时候改由应用决定
  （包括在主题切换后按当前调色板重建一遍）。
- 没有主题在位时，它**也一起不装**——与引擎对主题表的态度一致。
- 传空串撤销，`additionalStyleSheet()` 读回当前文本。

⚠️ 这与 4.6 那条「别在主题之上再叠一次 dock 整表」不矛盾：那条讲的是**同一批规则**
装两遍（附赠一张完整的 dock 表），这里讲的是**加在主题没说的地方**（应用自己的那几条）。

## 持久化

主题选择通过 @ref QxCore::QxSettings 存储：

| 键 | 内容 |
| :--- | :--- |
| `theme` | 主题的配置 id |
| `followSystem` | 是否跟随系统 |
| `systemLightTheme` / `systemDarkTheme` | 跟随系统时使用的配对 |

`settings()` 返回引擎持有的 @ref QxCore::QxSettings 对象；`setSettings()` 可以替换它
（所有权转移，已存的值会被重新读取），这也是让测试使用一个隔离配置文件的方式。

## 令牌：表里不写像素

生成的主题表不写裸值，写**令牌占位符**，由生成器在写出时替换成像素：

| 组 | 档位 |
| :--- | :--- |
| 圆角 `corner-` | `none` 0 · `sm` 2 · `md` 3 · `lg` 6 · `pill` 9999 |
| 字号 `type-` | `caption` 10 · `subtitle` 13 · `body` 15 · `title` 18 |
| 间距 `space-` | `xs` 4 · `sm` 8 · `md` 12 · `lg` 16 |
| 强调色 `accent-` 一族 | 每族一份：`accent` / `accent-hover` / `accent-fill` / `accent-faint` / `accent-text` … |

令牌的用处是让同一条规矩只写一处：五族 × 明暗十张表一起改，改动落在生成器里而不是十份文本里。
它同时是**可检查的**——raw 表里出现裸 `border-radius` 或数字字号，生成器直接拒写，
所以「有人顺手写了 7px」这类漂移进不来。

`qxdock` 的 `dockStyleSheet()` 回退片段认同一套占位符（写成 `{{corner-md}}`），渲染时替值。
**自己写片段、直接给像素的应用照常工作**：替换只命中 `{{...}}`，裸像素原样通过。

## 写自己的主题

1. 参照 `res/themes/` 下的样式表：同样是**一套骨架 + 一份调色板**——浅色主题描述 Ribbon 与
   dock，暗色主题额外把标准控件也重画（暗色下平台样式会忽略调色板）。顶点注释块写明了
   该主题取自哪套设计语言的调色板；表里用的是令牌而不是像素（见上一节）。
2. 调色板只由「深色 / 浅色」决定（@ref QxTheme::paletteFor），
   因此新增一套主题只需要注册时给对 `dark`，不需要改库。
3. 只想换样式表而不动注册，用 `setStyleSheet(id, css)` 即可，无需改库。
4. **样式表里不要再写图标**。`qxribbon` 自带的箭头、窗口按钮、对话框启动器等
   都是**单色掩膜**，颜色取自 @ref QPalette（如 `ButtonText`、`WindowText`），
   换主题时随调色板重绘；库只给一份资产，不再有明暗两套。
   应用自己若有单色掩膜资产，可以用 @ref QxRibbon::tintedIcon 按同一个规则着色。
5. **浅色主题的「按下」不填实色**。浅色主题的字形和文字都是深色（图标按 `text` 上色），
   把 `:pressed` 填成 `accent-fill` 就成了「深色字形压在中间蓝上」，既不是看不清的文字，
   也不是看得清的图标。浅色骨架因此把按下态做成比 hover 深一档的浅色填充、用 `accent` 描边，
   实色填充只留给内容本身是浅色的地方（应用按钮、gallery 选中项）；暗色骨架正相反——
   那里的字形本来就是浅色。这条不是约定而是门禁：`tools/themes/build_themes.py` 会对每张表
   的每个状态算前景/背景对比度，选错颜色时**生成直接失败**，不会写出表来。

