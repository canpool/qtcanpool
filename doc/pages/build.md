# 构建与安装

本页说明如何从源码构建 QtCanpool、如何选择组件、如何运行测试以及如何生成文档站点。

## 环境要求

| 项目 | 要求 |
| :--- | :--- |
| Qt | **5.15 及以上**，主推 Qt 6.5 / 6.8 LTS |
| C++ | C++17（构建框架强制 `CMAKE_CXX_STANDARD 17`，且不允许编译器扩展） |
| CMake | 3.16 及以上 |
| 编译器 | MinGW（随 Qt 分发）、MSVC 2019+、GCC 9+、Apple Clang |
| Doxygen（可选） | 1.9 及以上，仅在构建文档站点时需要 |
| Graphviz（可选） | 提供 `dot` 后类继承图才会生成 |

## 快速开始

```bash
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=<Qt安装目录>/<版本>/<编译器> \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release --parallel
```

`CMAKE_PREFIX_PATH` 是选用 Qt 的唯一入口，例如 `C:/Qt/6.8.3/mingw_64`。
也可以在 Qt Creator 中直接打开根目录的 `CMakeLists.txt`。

### 使用 CMake Presets（推荐）

根目录的 `CMakePresets.json` 固化了常用配置，省掉一长串 `-D`：

```bash
cmake --preset qt6           # Qt 6 + MinGW Makefiles
cmake --build --preset qt6
ctest --preset qt6           # 已带 QT_QPA_PLATFORM=offscreen

cmake --preset qt6-ninja     # 同上，但用 Ninja，构建更快
cmake --preset qt5           # Qt 5.15（尽力兼容）
cmake --preset coverage      # 覆盖率构建
cmake --preset sanitize      # ASan + UBSan
cmake --preset wasm          # Qt for WebAssembly
```

每个预设都带有默认路径，且可以用**环境变量覆盖而无需改文件**：

| 环境变量 | 用于预设 | 默认值 |
| :--- | :--- | :--- |
| `QTCANPOOL_QT_PREFIX` | `qt6` / `qt5` | `C:/Qt/6.8.3/mingw_64` / `C:/Qt/5.15.2/mingw81_64` |
| `QTCANPOOL_WASM_QT` | `wasm` | `C:/Qt/6.8.3/wasm_singlethread` |
| `QTCANPOOL_HOST_QT` | `wasm` | `C:/Qt/6.8.3/mingw_64` |
| `QTCANPOOL_EMSDK` | `wasm` | `C:/emsdk` |

```bash
QTCANPOOL_QT_PREFIX=/opt/Qt/6.8.3/gcc_64 cmake --preset qt6
```

> 预设里的默认路径是替作者本机准备的，并且都假设 Windows 布局。
> 在别的机器上设置对应环境变量即可，不必改动 `CMakePresets.json`。

### 生成器与构建树布局

单配置生成器（Ninja、Unix Makefiles、MinGW Makefiles）与多配置生成器
（Visual Studio、Xcode、Ninja Multi-Config）都可以用，但**一个构建目录只放一个配置**：

多配置生成器默认会把配置名追加到每个产物目录后面（`build/bin/Release`、`build/lib/qtproject/plugins/Release`），
而框架里所有相对路径与 `$ORIGIN` rpath 都是按**扁平**布局算的（`RELATIVE_PLUGIN_PATH` 正是其中之一）。
构建框架因此把每个配置的输出目录**钉在同一个位置**（`cmake/QtCanpoolAPI.cmake` 的 `qtc_pin_output_dirs()`），
让布局在两类生成器下一致。代价就是同一目录里放不下第二个配置——这与预设模型一致（一个预设一个构建目录）。

## 功能开关

开关均为 CMake 缓存变量，可在配置时通过 `-D<名字>=ON|OFF` 指定，
配置完成后用 `cmake -S . -B build -LH` 可以列出全部开关及当前取值。

| 开关 | 默认 | 说明 |
| :--- | :--- | :--- |
| `WITH_DEMOS` | `ON` | 构建 `demos/` 下的综合示例 |
| `WITH_TESTS` | `OFF` | 构建 `tests/` 下的单元测试并注册到 CTest |
| `WITH_DOCS` | `OFF` | 生成 Doxygen 文档站点（需要 Doxygen） |
| `WITH_ONLINE_DOCS` | `OFF` | 生成在线文档（qdoc 通道，遗留开关） |
| `BUILD_WITH_PCH` | `ON` | 使用预编译头加速构建 |
| `WITH_SANITIZE` | `OFF` | 打开检测器；种类由 `SANITIZE_FLAGS` 指定（例如 `address,undefined`） |
| `WITH_COVERAGE` | `OFF` | 打开覆盖率插桩（仅 GCC / Clang），产物交给 `lcov` |
| `BUILD_LINK_WITH_QT` | `OFF` | 作为 Qt Creator 的子项目构建时链接其 Qt |
| `SHOW_BUILD_DATE` | `OFF` | 在关于对话框中显示构建日期 |
| `ENABLE_SVG_SUPPORT` | 自动 | 检测到 Qt SVG 时自动打开 |

### 组件开关

每个库都可以单独关闭，关闭后依赖它的上层库会一并跳过，因此可以只构建需要的部分：

| 开关 | 默认 | 说明 |
| :--- | :--- | :--- |
| `WITH_QXCORE` | `ON` | 基础设施库（配置、日志） |
| `WITH_QXTHEME` | `ON` | 主题引擎（需要 `WITH_QXCORE`） |
| `WITH_QCANPOOL` | `ON` | legacy 核心库（已冻结） |
| `WITH_QXRIBBON` | `ON` | Ribbon 界面组件 |
| `WITH_QXDOCK` | `ON` | 可停靠窗口组件 |
| `WITH_QXWINDOW` | `ON` | 无边框自定义窗口组件 |
| `WITH_QXAPP` | `ON` | 应用框架（需要上面全部） |

## Qt 5 与 Qt 6

同一份源码同时支持两个大版本。构建框架中的 `cmake/FindQt5.cmake` 是一个 **Qt 6 优先的包装层**：
它把 `Qt5::X` 目标别名到 `Qt6::X`，因此库内部始终写 `Qt5::Core`、`Qt5::Widgets` 这类依赖，
实际链接到哪个版本取决于 `CMAKE_PREFIX_PATH`。

需要注意的少数差异：

- 高 DPI：Qt 6 默认开启，Qt 5 必须在构造 `QApplication` **之前**设置属性。
  `demos/qxapp/appshell/main.cpp` 给出了一份可直接复用的模板。
- `QAction` / `QActionGroup`：Qt 6 位于 `QtGui`，Qt 5 位于 `QtWidgets`。
  项目约定使用**不带模块前缀**的 `#include <QAction>`，两个版本都能解析。
- `Q_DECLARE_METATYPE`：Qt 5.15 下需要显式包含 `<QtCore/QMetaType>`。

## 运行测试

```bash
ctest --test-dir build -C Release --output-on-failure
```

测试通过 `add_qtc_test()` 注册，它为每个测试处理好 rpath、AUTOMOC 与头文件搜索路径
（公共包含目录是 `src/libs`，所以测试可以直接 `#include "qxribbon/xxx.h"`）。

在**没有交互桌面**的环境（CI、远程会话）中运行界面相关测试时，需要指定离屏平台：

```bash
QT_QPA_PLATFORM=offscreen ctest --test-dir build -C Release --output-on-failure
```

单个测试可以只跑一个用例类：

```bash
QTC_ONLY_TEST=tst_QxAppShell ./bin/tst_qxapp
```

## 覆盖率与动态分析

覆盖率与检测器都是**全局**开关：它们作用于树里的每一个目标。只插桩一部分只会得到
一个"看起来很干净"的假象。两者都仅支持 GCC / Clang，在其它编译器上会给出告警并忽略。

```bash
# 覆盖率
cmake --preset coverage
cmake --build --preset coverage
ctest --preset coverage
lcov --capture --directory build/coverage --output-file coverage.info \
     --ignore-errors mismatch,gcov,source
lcov --remove coverage.info '/usr/*' '*/tests/*' '*/Qt/*' \
     --output-file coverage.filtered.info --ignore-errors unused
lcov --list coverage.filtered.info

# ASan + UBSan
cmake --preset sanitize
cmake --build --preset sanitize
ctest --preset sanitize
```

> **覆盖率目前不是门禁**（决策 K8）：CI 会产出报告并作为构建产物上传，但不会因覆盖率
> 偏低而失败。等有了真实基线再谈阈值，避免为了凑数字去写无效测试。为了让"真实基线"真的存在，
> CI 除了把 `coverage.filtered.info` 作为 artifact 上传，还把 `lcov --list` 的结果写进**作业摘要页**
> ——artifact 要下载一次，而需要下载的数字没人会拿来逐版本对比（4.1 的决策 K22）。

CI 把质量检查拆成两个作业，因为「门禁」和「只报告」不该互相拖累：

| 作业 | 内容 | 是否阻塞 |
| :--- | :--- | :--- |
| `quality` | 覆盖率（lcov 报告 → artifact + 作业摘要）、ASan + UBSan 跑完整 ctest | **是**（覆盖率按 K8 不设阈值，只有测试失败才红） |
| `tidy` | clang-tidy 静态分析 | **否**（`continue-on-error`，报告只进作业摘要与 artifact） |

`tidy` **单独一个作业**是有意的：它原来跟在覆盖率与消毒器之后，等于让「不阻塞」的分析挡在所有
结果的墙钟时间前面。拆出去以后它与前两者并行跑，而且**只 configure 不编译**——clang-tidy 只需要
`compile_commands.json`，不需要任何目标文件。

clang-tidy **只报告不阻塞**——它面对的是一棵从未被静态分析过的树，在既有告警被甄别完之前就设成门禁，
只会把人训练成忽略红灯。摘要（按 check 分组的计数 + 前 20 条）直接写进 GitHub 的作业摘要页，
完整日志仍作为 `clang-tidy-log` artifact 保留。

范围分三种，取决于这次改动动了什么：

| 改动 | 分析范围 |
| :--- | :--- |
| 只动了 `.cpp` | **改动到的那些行**——`scripts/tidy_line_filter.py` 从 diff 的 hunk 生成 clang-tidy 的 `-line-filter`，落在改动行之外的告警不报 |
| 动了 `.h` | 整棵树，**不加行过滤**：头文件能波到每一个翻译单元，而被它引起的告警会出现在某个 `.cpp` 的**未改动行**上，按行过滤恰好会把这份波漏掉 |
| 没有可比对的基点（新分支） | 整棵树 |

为什么按**行**而不是按**文件**：`src/libs` 现在带着**几百条** `bugprone-*` / `performance-*` 告警
（本机 clang-tidy 21.1.2 实测 183 条，"窄化转换"一类就占 81 条）。按文件筛会把同文件里的历史告警
一起算到本次改动头上，"引入了什么"这个数字就废了。按行筛才是它字面的意思。
过滤之后仍**不门禁**——先把测量做准，阈值留到有数字可谈的时候（K22）。

## 构建文档站点

`doc/` 目录本身是一个独立的 CMake 工程，只依赖 Doxygen 与可选的 Graphviz，**不需要 Qt**：

```bash
cmake -S doc -B build-docs
cmake --build build-docs --target docs
# 产物：build-docs/html/index.html
```

也可以作为主构建的一部分生成：

```bash
cmake -S . -B build -DWITH_DOCS=ON -DCMAKE_PREFIX_PATH=<Qt>
cmake --build build --target docs
```

可用的文档相关缓存变量：

| 变量 | 说明 |
| :--- | :--- |
| `DOXYGEN_EXECUTABLE` | 指定要使用的 Doxygen 可执行文件 |
| `DOXYGEN_PROJECT_NUMBER` | 覆盖页面中显示的版本号（默认取 `cmake/QtCanpoolBranding.cmake`） |
| `DOXYGEN_OUTPUT_LANGUAGE` | 站点界面语言，默认 `Chinese` |
| `DOXYGEN_WARN_AS_ERROR` | 设为 `YES` 或 `FAIL_ON_WARNINGS` 可让告警直接导致构建失败 |

## 示例程序

构建完成后，可执行文件位于构建目录的 `bin/`（Windows）；
在其它平台上它们位于 `libexec/qtproject/`（与安装布局一致）：

| 示例 | 目标 | 说明 |
| :--- | :--- | :--- |
| `ribbondemo` | `RibbonDemo` | Ribbon 界面：页、分组、快捷工具栏、应用按钮 |
| `dockdemo` | `DockDemo` | 可停靠窗口：中央区域、浮动、标签化、布局保存 |
| `appshell` | `AppShellDemo` | 应用外壳：导航轨 + 页面栈 + 停靠区 + 主题 + 启动屏 + 布局持久化 |

`examples/` 下另有 22 个更细粒度的控件示例，同样由 CMake 构建，可用
`-DWITH_EXAMPLES=OFF` 关掉（默认随构建一起编译，但不安装）。

## 安装与消费

```bash
cmake --install build --prefix <安装目录>
```

头文件安装到 `<prefix>/include/qtproject`，库与可执行文件安装到 `<prefix>/lib`
（可执行文件在 Windows 的 `<prefix>/bin`、其它平台的 `<prefix>/libexec/qtproject`）。

> ⚠️ **公开头文件与 CMake 包配置位于带 `EXCLUDE_FROM_ALL` 的 `Devel` 组件**，
> 普通安装不会装出它们，必须再执行一次组件安装：
>
> ```bash
> cmake --install build --prefix <安装目录> --component Devel
> ```

安装完成后即导出 CMake 包配置，下游工程可以 `find_package(QtCanpool)` 后
直接链接 `QtCanpool::qxcore` / `QtCanpool::qxtheme` /
`QtCanpool::qxribbon` / `QtCanpool::qxdock` / `QtCanpool::qxwindow` /
`QtCanpool::qxapp` 这些目标：

```cmake
find_package(QtCanpool REQUIRED)
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE QtCanpool::qxapp)
```

仓库里的 [`projects/consume`](https://github.com/canpool/qtcanpool/tree/master/projects/consume)
是一个最小下游工程，CI 在 `ubuntu-latest / Qt 6.8.1` 上执行完整的
"安装 → `find_package` → 编译链接 → 运行" 往返，作为可消费性的门禁。
此外还提供 [vcpkg / Conan 骨架](https://github.com/canpool/qtcanpool/tree/master/ports)
（未经 CI 验证，供社区贡献）。

## 新建工程

生成一个新工程用 `scripts/new-project`，它会产出一个自包含的 CMake 工程
（`QxAppShell` 窗口 + 一个页面 + 图标资源 + `.gitignore`）：

```bash
python scripts/new-project myapp --build \
  --sdk <QtCanpool 安装目录> --qt <Qt 安装目录>
```

不要 `--build` 时只生成，并打印出随后要执行的配置/编译命令（与 `--build` 实际执行的是同一组命令）。
路径也可以预先用环境变量给出：`QTCANPOOL_SDK`、`QTCANPOOL_QT_PREFIX`；Windows 上 CMake 默认选择
Visual Studio 生成器，若未安装 MSVC 需用 `-G Ninja`（或 `-G "MinGW Makefiles"`）显式指定生成器。

生成的工程只依赖**已安装的** SDK：`find_package(QtCanpool REQUIRED)` 加一条
`QtCanpool::qxapp` 链接，不引用本仓库源码。

不装 SDK 而想先看看写法，读 [`projects/template`](https://github.com/canpool/qtcanpool/tree/master/projects/template)
（CMake 版）。

## 关于 qmake

**qmake 已在 4.0 移除**，全树不再有 `.pro` / `.pri`，CMake 是唯一的构建方式。

此前它在 3.0 被宣布为"冻结（只读）"，但因为 CI 从未构建过 qmake，冻结无人守护——
`qxapp` 的 `-lib.pri` 漏了 8 个源文件，其中就包括 `QxAppShell` 本体，
用 qmake 编出来的库里根本没有主窗口类。既然这份"兼容"已经名存实亡，
K13 决定在 4.0 将其删除，而不是继续留着一批会腐烂的文件。

> **迁移**：qmake 工程请改走 CMake——安装 SDK 后 `find_package(QtCanpool)` 再
> 链接 `QtCanpool::qxapp` 等目标即可，见 [`projects/consume`](https://github.com/canpool/qtcanpool/tree/master/projects/consume)
> 与 [`projects/template`](https://github.com/canpool/qtcanpool/tree/master/projects/template)。
