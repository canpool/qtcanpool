演示 4.5 的原生 OS 拖拽（C3）：`DockDrag` 用自有 mime `application/x-qxdock-dockwidget` 把停靠件作为**原生拖拽**携带，载荷是 UTF-8 文本 `QxDock/1\n<name>\n<title>`，便于他进程/非本库接收方直读。

- `DockWindow` 只接受该 mime 的投放，与既有进程内鼠标拖拽（根本不产生 `QDrag`）零交叉。
- 本例的 `Notes` 停靠件可关闭（默认不含 `DeleteOnClose`，对象保留）。用 `File > Drag Notes out (native)` 发起原生拖拽，把它拖回窗口：窗口找到自己名下已关闭的 `Notes` 并复原，同时通过 `dockWidgetDropped(name, globalPos)` 信号上报——状态栏会打印名字与落点。
- 跨进程场景：从另一个基于 qxdock 的程序拖出一个停靠件落到本窗口，本窗口只认 mime、无法复原对方部件（由应用自己按名字重建），但同样会触发 `dockWidgetDropped` 信号。
