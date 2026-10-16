演示 RibbonBar 的多套样式切换（Office / Wps / 两行）以及快速访问栏左右位置、group 标题显隐的实时切换。

此例同时是以下 4.5 功能的现成载体（均内建在 `RibbonBar`，无需额外代码即可观察）：

- **B1 KeyTips**：按 `Alt` 给应用按钮与可见页签标注字母角标；按页签字母下探到该页按钮，再按字母触发控件，`Esc`/`Alt` 退出。
- **B6 ScreenTip**：把鼠标停在按钮上，显示富提示（去助记符的文本 + statusTip + 快捷键）；无描述且无快捷键时退回普通 tooltip。
- **B8 快速访问栏定制入口**：右键点快速访问栏的定制菜单（或下拉箭头），末尾的「Customize Quick Access Toolbar...」会触发 `customizeDialogRequested()` 信号，本例在此打开一个宿主侧定制对话框。
- **B10 右键上下文菜单**：在 Ribbon 上右键弹出上下文菜单（含「Minimize the Ribbon」与快速访问栏定制入口），子类可覆写 `addContextMenuActions()` 扩展。
