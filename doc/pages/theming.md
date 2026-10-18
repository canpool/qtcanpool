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

应用主题只做两件事：`QApplication::setPalette()` 与 `QApplication::setStyleSheet()`。
Qt 随后会向所有部件派发 `QEvent::StyleChange`，而 `qxribbon` 与 `qxdock` 早已有为该事件
准备的既有处理（例如 `RibbonBar` 收到后重排整条 Ribbon —— 标签条的宽度是把它自己的标签
加起来算出来的，换一张表标签变宽了就得重算；`DockTab` 重刷样式）。
因此引擎不需要知道有哪些窗口存在。

⚠️ **这两步的顺序是调色板先、样式表后，不能反过来**。应用级样式表会让 Qt 给被它命中的
每个部件换上**该部件自己的**调色板，此后 `QApplication::setPalette()` 就再也到不了它们；
样式表最后落下，装它的这一步同时就是让这些部件按新调色板重新 polish 的时刻。
先表后板在「亮 → 暗」时往往看不出来（样式表本身把颜色写成了暗色），在「暗 → 亮」时就会
把编辑器这类部件的底色留在上一套主题里。`QxThemeManager` 一直按这个顺序装；自己手写
`setPalette` + `setStyleSheet` 的应用照抄这个顺序即可。

也正因为是 **应用级**下发：把样式表贴在窗口自己身上会盖掉应用的那张，
这正是 4.5 之前 Ribbon 区不跟随主题的原因。窗口不带主题，应用下发的才到得了 Ribbon。

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

## 自定义样式表

样式表是注册信息的一部分，也可以事后替换：

```cpp
// 直接给出内容（传空串可撤销）
themes->setStyleSheet(QStringLiteral("dark-officeplus"), myCss);

// styleSheet(id) 可以查看 apply() 实际会装载的内容
```

## 持久化

主题选择通过 @ref QxCore::QxSettings 存储：

| 键 | 内容 |
| :--- | :--- |
| `theme` | 主题的配置 id |
| `followSystem` | 是否跟随系统 |
| `systemLightTheme` / `systemDarkTheme` | 跟随系统时使用的配对 |

`settings()` 返回引擎持有的 @ref QxCore::QxSettings 对象；`setSettings()` 可以替换它
（所有权转移，已存的值会被重新读取），这也是让测试使用一个隔离配置文件的方式。

## 写自己的主题

1. 参照 `res/themes/` 下的样式表：同样是**一套骨架 + 一份调色板**——浅色主题只描述
   Ribbon，暗色主题额外把标准控件也重画（暗色下平台样式会忽略调色板）。
   顶点注释块写明了该主题取自哪套设计语言的调色板。
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

