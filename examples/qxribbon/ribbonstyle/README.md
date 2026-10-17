通过右上角 RibbonButtonGroup 配合 RibbonMenu 下拉切换 Ribbon 的两类外观，二者处于**两个不同的层级**：

- **布局样式**（Office / Wps / 两行）：`RibbonBar::setRibbonStyle()`。它决定 RibbonBar 自己怎么排布页与组，因此是**该控件**的属性。
- **配色主题**（Office 2013 / Fancy / Dark Office Plus / Fluent Light / Fluent Dark）：由**应用注册、按 id 选用**。样式表是本例自带的（`res/themes.qrc` 下的 `:/themes/*.css`），本例把它们注册进 `QxThemeManager`，菜单项直接由 `themeIds()` 生成，选中时只**报一个 id**；调色板、样式表下发、下次启动的记忆都由引擎负责。

也就是说：`qxribbon` 不自带任何主题，Ribbon 用什么颜色取决于**应用下发了什么**。状态栏会显示当前主题名与明暗，暗色主题同时意味着暗色调色板——这正是那些样式表没有描述到的控件（编辑器、树、输入控件…）所取色的地方。

切换 Office 2013 与 Dark Office Plus 可以观察：整窗（含 Ribbon 区）一起变，而不是只有 Ribbon 变。
