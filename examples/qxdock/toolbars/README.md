演示「**可停靠工具栏 + 停靠布局**」的组合方式：`DockWindow` 放中央区，工具栏由宿主 `QMainWindow` 提供。

- **无需本库提供工具栏区**：多行、自动重排、浮动、跨栏拖拽 `QMainWindow` 已实现；qxdock 若自造一套，既要复刻它的布局与拖拽机制，又得和它对「什么算一行」达成一致。
- **两半布局分属两个所有者**：`QMainWindow::saveState()` 管工具栏与主窗，`DockWindow::saveStateToFile()` 管停靠件；菜单里的 Save/Restore 正是分别调用两者。
- 工具栏的 `objectName` 是 `QMainWindow::saveState()` 记录它时的标识，必须设置。
