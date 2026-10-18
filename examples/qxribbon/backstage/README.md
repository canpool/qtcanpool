通过左上角应用按钮打开 Backstage 视图（`RibbonBackstageView`，4.5 B4 新增）：

- **覆盖宿主**：视图构造时传入所属 `RibbonBar`，自动覆盖 ribbon 的父窗口（本示例即整个窗口），并跟随窗口缩放；点击应用按钮开/合，`Esc` 或列顶的返回按钮关闭。
- **左列条目**：命令（`addAction`，点击触发 action）、分隔条（`addSeparator`）、页条目（`addPage`）三类，按添加顺序排列；条目的 QSS 对象名（`qx_RibbonBackstage*`）与主题其余部分一致，各主题均已适配。
- **右侧页面**：`addPage` 接受宿主任意 widget（示例为两个只读文本页 + 一个带勾选框的 Options 页），条目文字取 widget 的 windowTitle；当前页的条目为 checked 态。
- **阻止关闭**：Options 页的复选框即 `setClosePrevented()` 的现场演示——勾上后 `Esc` 与返回按钮均不再关闭面板，由宿主自行处理后再关闭。

信号 `aboutToShow()` / `aboutToHide()` 在开合后各发一次，示例里接状态栏提示。
