### 工程说明

- `template/` —— **新项目模板**。以 CMake 为主（`CMakeLists.txt` + `main.cpp`，基于
  `QxAppShell`，通过 `find_package(QtCanpool)` 消费已安装的 SDK）；同目录的
  `.pro`/`.pri`/`src/` 是 qmake legacy 参考，只读保留。用法见 `template/README.md`。
- `consume/` —— 下游 SDK 消费验证工程：只依赖安装前缀，被 CI 用于验证「安装 → 消费」
  全链路。见 `consume/README.md`。
- `staticlink/` —— 静态链接示例（qmake）。
