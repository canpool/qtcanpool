### 工程说明

- `template/` —— **新项目模板**，同时是 `scripts/new-project` 的渲染输出：基于 `QxAppShell`
  的 `CMakeLists.txt` + `main.cpp` + `resources/`（图标资源）+ `.gitignore`，通过
  `find_package(QtCanpool)` 消费已安装的 SDK（原先并排的 qmake 版本已于 3.2 删除）。
  生成器是这项定义的唯一出处（决策 K9），这份目录由它写出，`scripts/check-template-sync.py`
  逐字节比对二者并在 CI 里作为门禁。用法见 `template/README.md`。
- `consume/` —— 下游 SDK 消费验证工程：只依赖安装前缀，被 CI 用于验证「安装 → 消费」
  全链路。见 `consume/README.md`。
- `plugin/` —— **外部插件验收工程**（路线图 DoD #1）：不碰源码树，只用 `find_package(QtCanpool)`
  造出一个插件，并再写一个最小宿主把它真正加载起来，证明它「可用」而不只是「能编译」。
  见 `plugin/README.md`。

> `staticlink/`（静态链接示例）已于 4.0 随 qmake 一并删除：它只有一份 qmake 工程文件
> 和一个空的 `QMainWindow`，没有用到任何 QtCanpool API，表达不了静态链接之外的东西，
> 而那部分也已由 `template` / `consume` 覆盖。
