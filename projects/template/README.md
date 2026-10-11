# template —— 新项目模板

以 CMake 为主的起点工程：一个基于 `QxAppShell` 的最小应用，通过
`find_package(QtCanpool)` 消费**已安装**的 SDK —— 不引用源码树，不做
`add_subdirectory`，这正是下游工程的样子。

> **本目录是 `scripts/new-project` 的渲染结果**（用的工程名是 `qxtemplate`），不是第二份手工
> 维护的模板：生成器是这项定义的唯一出处（决策 K9），这里的文件由它写出，
> `scripts/check-template-sync.py` 逐字节比对二者，并在 CI 里作为门禁。
> 要改请先改 `scripts/new-project`，再跑 `python3 scripts/check-template-sync.py --update`
> 重新渲染本目录。
>
> 比对豁免一个文件：`README.md`。脚本生成的那份写给新工程的主人，本文件写给正在读这个仓库的人
> —— 二者都留，谁也替不了谁。

> **3.2 起本目录只有 CMake 模板。** 原先并排的 qmake 版本（`template.pro` / `config.pri` /
> `qtproject.pri` / `src/`）连同它引用的 `qcanpool` 库一起删除——qmake 自 3.0 起冻结，
> 而那个 qmake 应用实际未使用任何 QtCanpool API，留一份不能构建的参考只会误导。

## 1. 准备 SDK

```bash
# 在仓库根目录：配置、编译、安装到一个前缀
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
cmake --install build --config Release --prefix /path/to/sdk
cmake --install build --config Release --prefix /path/to/sdk --component Devel
```

> 头文件与 `QtCanpoolConfig.cmake` 属于 `Devel` 组件（`EXCLUDE_FROM_ALL`），所以需要
> **显式**安装该组件，否则只有可执行文件与运行库落到前缀里。

## 2. 构建与运行

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="/path/to/sdk;/path/to/Qt"
cmake --build build --parallel
./build/qxtemplate        # Windows: build\qxtemplate.exe
```

`CMAKE_PREFIX_PATH` 里**必须同时有 SDK 前缀与 Qt**：`QtCanpoolConfig.cmake` 会
`find_dependency(Qt5 ... COMPONENTS Core Gui Widgets)`。Linux/macOS 下分隔符是 `:`，
Windows 下是 `;`。

## 3. 从这里开始改

- `main.cpp`：把示例页换成自己的部件；完整接口见 `qxapp/qxappshell.h` 的头注释。
- 开箱已接好三件事，先看它们跑起来再改：

  | 能力     | 入口                                                       |
  | :----- | :------------------------------------------------------- |
  | 切换语言   | 设置对话框的 **Language** 项（枚举来自可执行文件旁的 `*_<语言>.qm`）         |
  | 应用内通知  | 启动时的欢迎 Toast；应用设置后再弹一条                                   |
  | 打开设置界面 | 功能区 **Home → Application → Settings** 按钮                  |

  三者都在 `main.cpp` 里，各约十几行：语言交给 `QxTranslator`、通知交给
  `QxAppShell::showToast()`、设置界面对话框由 `QxProperty` 描述后交给 `QxSettingsDialog`。
  `scripts/new-project` 生成的工程与这里**逐字节相同**（只有 `README.md` 不同），所以在新工程里
  读到的写法与这里看到的写法不会分家。
- 需要自己的库时：建子目录 + 自己的 `CMakeLists.txt`，
  在根 `CMakeLists.txt` 里 `add_subdirectory`，然后 `target_link_libraries` 链接。
  只有在该库的**公开头**里暴露 QtCanpool 类型时才用 `PUBLIC`，否则用 `PRIVATE`。
- 安装/发布规则：模板刻意不带 `install()`，保持最小。需要时给目标补
  `install(TARGETS ...)` 即可 —— 导出规则由 SDK 提供，无需自己写 `EXPORT`/`Config`。

## 文件

| 文件                          | 说明                                            |
| :-------------------------- | :-------------------------------------------- |
| `CMakeLists.txt`            | CMake 构建：一个可执行文件，一条 `target_link_libraries` |
| `main.cpp`                  | `QxAppShell` 最小示例                            |
| `resources/app.qrc`         | Qt 资源清单，把 `logo.png` 挂到前缀 `/qxtemplate`       |
| `resources/images/logo.png` | 生成器内联的 32×32 占位图标（`new-project` 的 `LOGO_PNG_B64`） |
| `.gitignore`                | 忽略构建目录与 Qt Creator 残留                        |
| `README.md`                 | 本文件（比对豁免，见开头）                                 |

> `main.cpp` 用 `QApplication::setWindowIcon(QIcon(":/qxtemplate/images/logo.png"))` 取这个图标，
> 换成自己的图只需要替换 `resources/images/` 下的文件，或改 `app.qrc` 与 `kLogoPath` 常量。
