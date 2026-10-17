# 文档站点（Doxygen）

`doc/` 是**可独立构建**的 Doxygen 工程：不需要 Qt / 编译器（`LANGUAGES NONE`）。输入 =
`src/libs`（API）+ `doc/pages/*.md`（指南）。CI 的构建与部署见 [CI 与在线站点](ci.md)。

## 构建

```bash
cmake -S doc -B build-docs && cmake --build build-docs --target docs
# 产物：build-docs/html/index.html
```

也可 `-DWITH_DOCS=ON` 并入主构建（见 [构建与安装](build.md#构建文档站点)）。刻意不走 qdoc：
`cmake/QtCanpoolDocumentation.cmake` 是 Qt Creator 遗留死代码，保留不删。

## Doxyfile 关键项

- `EXPAND_ONLY_PREDEF = NO`：否则命名空间宏不展开、`\ref` 失效。
- `INCLUDE_PATH = src/libs`：让 Doxygen 找到被引用的头。

## 几个容易踩的坑

- ⚠️ **自定义样式表不能叫 `doxygen.css`**（Doxygen 自带表名）：1.9.8 报 error 并**静默丢弃**
  （build 仍绿）→ 用 `doc/qtcanpool-doxygen.css`。**构建不报错 ≠ 定制生效，必须去产物 grep 标记确认。**
- ⚠️ **Doxygen 里 `Q_OS_*` 全未定义**：`qxwindow/windowcontext_p.h` 用 `#if/#elif/#else` 三后端三选一
  → `PREDEFINED` 加 `Q_OS_MAC=1`（保 Qt 后端 + 暴露 macOS 公开 API），`EXCLUDE` 掉
  `windowcontext_win.cpp`；**不要**同时定义 `Q_OS_WINDOWS`。
- ⚠️ **注释别被「绑到 define」**：紧贴 `#ifndef`/`#define` 的注释块会成为该宏的文档，块内 `@ref` 不再
  解析；`@def <不存在名>` 会让整块文档被丢弃——**警告消失也可能是文档丢了**。

## 现状

- **968 页、警告 0**（计数用 `grep -cE ': (warning|error):'`，别数日志行数，续行会虚高约三成）。
- 本机 Doxygen（MSYS2 手工拼装）退出时崩溃（`std::system_error`，码 3）但产物完整；CI 发行版无此问题。
- 线上：<https://canpool.github.io/qtcanpool/>（GitHub Pages）。
