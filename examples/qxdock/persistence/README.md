演示 4.5 的停靠布局文件持久化（C2）：`DockWindow::saveStateToFile()` / `restoreStateFromFile()` 把布局写成**不压缩的纯 XML** 文件。

- `saveState()` 底层原本就是 XML，但默认压缩成不透明的 `QByteArray`，只能进 `QSettings`；这两个新方法复用同一套写入逻辑、只要求「不压缩」，于是文件人可以打开、手改、diff。
- 空路径、打不开、版本不符都返回 `false` 且不动现有布局。
- 本例在启动时会尝试恢复上次保存的布局（默认落在系统临时目录的 `qxdock_persistence_demo.xml`）；用 `File > Save layout...` / `Restore layout...` 可在运行时存取。
