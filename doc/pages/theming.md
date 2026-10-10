# 主题引擎

`qxtheme` 提供进程级的主题引擎，即 @ref QxTheme::QxThemeManager

## 它解决什么问题

在引擎出现之前，主题能力是互不相干的两半：

- `RibbonTheme`（qxribbon）能装载样式表，但**没有调色板**、**不持久化**、**不跟随系统**；
- `WindowStyleAgent`（qxwindow）只能**探测**系统明暗，并不负责应用。

结果是菜单、原生对话框、工具提示、禁用态文字这些样式表覆盖不到的表面停留在平台默认色，
和深色样式表明显不搭。`QxThemeManager` 把「调色板」与「样式表」作为一件事一起应用，
并补上持久化与系统跟随。

## 内置主题

| 枚举 | 配置 id | 显示名 | 深色 |
| :--- | :--- | :--- | :---: |
| `LightYellow` | `light-yellow` | Light Yellow | |
| `LightOffice2013` | `light-office2013` | Light Office 2013 | |
| `LightClassic` | `light-classic` | Light Classic | |
| `LightFancy` | `light-fancy` | Light Fancy | |
| `DarkWps` | `dark-wps` | Dark WPS | ✓ |
| `DarkOfficePlus` | `dark-officeplus` | Dark Office Plus | ✓ |
| `Custom` | `custom` | Custom | |

枚举顺序与 `RibbonTheme::ThemeStyle` 一致，两者无需转换表即可对应。
**配置 id 是对外契约**：它被写进配置文件，改动需要配合配置迁移。

辅助函数：

| 函数 | 说明 |
| :--- | :--- |
| @ref QxTheme::themeId | 取稳定 id |
| @ref QxTheme::themeName | 取显示名，适合放进设置对话框 |
| @ref QxTheme::themeFromId | 由 id 反查枚举，未知 id 返回给定的回退值 |
| @ref QxTheme::isDarkTheme | 是否为深色主题 |
| @ref QxTheme::themeStyleSheetPath | 内置样式表的资源路径（`Custom` 为空） |
| @ref QxTheme::themeStyleSheet | 内置样式表的内容，资源缺失时为空 |

## 基本用法

在 `QApplication` 构造完成之后取得单例即可，选择会被持久化，下次启动自动恢复：

```cpp
#include "qxtheme/qxthememanager.h"

using namespace QxTheme;

QxThemeManager *themes = QxThemeManager::instance();
themes->setTheme(DarkOfficePlus);

if (themes->isDark()) {
    // 例如切换图标集
}
```

`setTheme()` 会立即应用并发出 `themeChanged()`；`apply()` 用于把当前主题重新装一遍
（比如在外部改动了样式表之后）。

## 运行时切换为什么不需要遍历控件

应用主题只做两件事：`QApplication::setPalette()` 与 `QApplication::setStyleSheet()`。
Qt 随后会向所有部件派发 `QEvent::StyleChange`，而 `qxribbon` 与 `qxdock` 早已有为该事件
准备的既有处理（例如 `RibbonBar` 重新读取主题、`DockTab` 重刷样式）。
因此引擎不需要知道有哪些窗口存在。

## 跟随系统深浅色

```cpp
themes->setSystemThemePair(LightOffice2013, DarkOfficePlus);  // 可选，覆盖默认配对
themes->setFollowSystemTheme(true);
```

默认配对是 `LightOffice2013` / `DarkOfficePlus`。开启跟随之后：

- 立即应用与当前系统匹配的主题；
- 之后系统切换时自动跟随（Qt 6.5+ 通过 `QStyleHints::colorSchemeChanged`）；
- **持久化的是"跟随"这个开关本身，而不是它推导出的主题**，因此用户此后仍能显式选一个主题；
- 在不报告系统色方案的平台上，可以手动调用 `refreshSystemTheme()` 重新评估。

## 自定义样式表

内置样式表位于 `:/qxribbon/res/stylesheets/*.css`，是 qxribbon 的资源。
引擎**不链接 qxribbon**：资源是进程级注册的，只要 qxribbon 被链接进进程，按路径就能读到。
所以 qxribbon 一侧零改动，非 Ribbon 应用也可以使用主题引擎。

资源不可用时（例如没链接 qxribbon），引擎**只应用调色板并告警一次**，
而不会清空样式表 —— 清空会抹掉调用方自己设置的样式。

要提供自己的样式表，两种方式：

```cpp
// 指向一个外部文件（会持久化）
themes->setStyleSheetFile(DarkOfficePlus, QStringLiteral(":/myapp/dark.css"));

// 直接给出内容，优先级高于文件（传空串可撤销覆盖）
themes->setStyleSheet(Custom, myCss);
```

`Custom` 主题没有内置资源，配合上面两种方式使用最合适。
`styleSheet(theme)` 可以查看 `apply()` 实际会装载的内容。

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

1. 参照 `qxribbon/res/stylesheets/` 下的样式表，其顶部注释块列出了该主题的
   `qtcanpool-qss` 设计令牌（背景、前景、强调色、边框等）。
2. 调色板取自同一组令牌，因此新增内置主题时需要同步在 `qxthemepalette.cpp` 中补上色值。
3. 只想换样式表而不改调色板时，用 `Custom` + `setStyleSheetFile()` 即可，无需改库。

> **注意**：`RibbonTheme` 仍然存在，是 widget 级的样式表加载器。
> 让它转发到引擎会引入 `qxribbon → qxtheme` 的库依赖，需要先确认这一决策。
