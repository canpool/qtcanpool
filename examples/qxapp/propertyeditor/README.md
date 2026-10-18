qxapp 属性与设置系统样例：用 `QxProperty` 描述设置项，由 `QxPropertyEditor` 编成表单，再交给 `QxSettingsDialog` 以「左侧页列表 + 右侧表单」呈现，改动经 `QxSettings` 落盘（`qtcanpool_example.ini`）。

窗口中央就是 `QxPropertyEditor`：`addGroup()` 把设置分成 Appearance / Editor 两段，启动时先用 `setAllValues()` 把 ini 里存的值灌进去作为基线（没有就取 `defaultValue`）。底部三个按钮各演示一种「回到正常」：

- **Apply** —— 把 `allValues()` 写回 `QxSettings` 并 `sync()`，然后把刚写的值设为新基线；
- **Restore defaults** —— `reset()`，回到声明的 `defaultValue`，即全新安装的样子；
- **Discard edits** —— `resetToInitial()`，回到 `setAllValues()` 给的基线，即「撤销我刚敲的」。

`modifiedChanged()` 驱动 Apply 是否可用与状态文字，`valueChanged()` 把最后改动的键显示在状态栏。菜单栏的 *Settings dialog...* 打开 `QxSettingsDialog`，它写的是同一个 `QxSettings` 对象，关闭后表单会把对话里改动的值重新读成基线——两种视图因此始终一致。
