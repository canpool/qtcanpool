演示 RibbonBar 的多套样式切换（Office / Wps / 两行）、快速访问栏左右位置与 group 标题显隐的实时切换，以及 ScreenTip 的两种形态。

此例同时是以下 4.5 功能的现成载体（均内建在 `RibbonBar`，无需额外代码即可观察）：

- **B1 KeyTips**：按 `Alt` 给应用按钮与可见页签标注字母角标；按页签字母下探到该页按钮，再按字母触发控件，`Esc`/`Alt` 退出。
- **B6 ScreenTip**：page1 的「screen tip」组带了三类 action——`save`/`undo` 有 statusTip + 快捷键，悬停显示富提示（去助记符的文本 + 描述 + 快捷键）；`plain` 只有普通 toolTip，悬停仍是普通 tooltip。两者对照即 B6 的触发条件（action 带 statusTip 或快捷键才升级为富提示）。
- **B8 快速访问栏定制入口**：右键点快速访问栏的定制菜单（或下拉箭头），末尾的「Customize Quick Access Toolbar...」会触发 `customizeDialogRequested()` 信号，本例在此打开一个宿主侧定制对话框。
- **B10 右键上下文菜单**：在 Ribbon 上右键弹出上下文菜单（含「Minimize the Ribbon」与快速访问栏定制入口），子类可覆写 `addContextMenuActions()` 扩展。
