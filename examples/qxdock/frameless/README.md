演示「**无边框主窗 + `DockWindow`**」的宿主侧写法：停靠、浮动、自动隐藏与有边框主窗完全一致，缺的只是边框本身，由宿主补回。

- 标题栏：无边框就无框可拖，所以宿主自己提供标题栏（拖动移动窗口、双击最大化/还原）。
- 边框：布局留出 4px 边框条，鼠标按在边缘时交给 `QWindow::startSystemResize()` —— 这一条**必须**属于宿主，因为按在停靠区内的鼠标事件会先到 `DockWindow`（拖拽停靠正是靠它），不能在库里去抢。
- 若宿主已有无边框主窗（例如 `qxwindow` 的），两项它都已具备，`DockWindow` 直接放进去即可。

`DockWindow` 侧无需任何改动，见 `tests/qxdock/auto/tst_dockwindow.cpp` 的 `framelessHost()`。
