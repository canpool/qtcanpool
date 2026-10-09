# qtcanpool 3.0 开发规划

> 版本定位：**现代化收敛版（Modern Consolidation）**  
> 主题：**Qt6 优先、CMake 优先、组件化收敛、工程化补齐**  
> 文档状态：草案 v1 · 待评审  
> 适用仓库：`https://github.com/canpool/qtcanpool` / `https://gitee.com/icanpool/qtcanpool`

---

## 一、项目现状分析

### 1.1 定位

qtcanpool 是一套**源自 QtCreator 源码结构**的通用 Qt 项目管理模板：

- **项目管理层面**：提供多项目（`projects/`）、多示例（`demos/`、`examples/`）、测试（`tests/`）、第三方集成（`thirdparty/`）的目录范式；
- **核心库层面**：以 `qcanpool`（基于 QtWidgets）为核心，配套 `qxribbon` / `qxdock` / `qxwindow` / `qxwidget` 等组件库；
- **构建层面**：同时支持 **qmake** 与 **CMake** 两套构建体系。

一句话：它既是 **UI 组件库**，也是一套 **"多项目单仓管理"的工程模板**。

### 1.2 架构分层

```
qtcanpool/
├── cmake/          CMake 框架（源自 QtCreator，含 Branding/Translations/API 等模块）
├── scripts/        deployqt.py 等辅助脚本
├── src/
│   ├── libs/       ★ 核心库（全部业务价值集中于此）
│   │   ├── qcanpool   legacy 控件库（fancy* 系列 + 简易 ribbon）
│   │   ├── qxribbon   现代 Ribbon 控件库
│   │   ├── qxdock     停靠系统（源自 Qt-Advanced-Docking-System）
│   │   ├── qxwindow   无边框窗口套件（源自 qwindowkit）
│   │   └── qxwidget   应用框架（RibbonAppWindow = ribbon + window，新建）
│   ├── modules/    基础模块（当前为空）
│   ├── plugins/    插件框架（当前仅有骨架）
│   ├── shared/     共享 PCH / 头文件
│   └── tools/      工具
├── demos/          重量级演示（fancydemo / fancyribbon / dockdemo / ribbondemo / qxwindowdemo）
├── examples/       细粒度控件示例（qcanpool 8 例、qxdock 10 例、qxribbon 13 例、qxwidget 1 例）
├── tests/          测试（helloworld、qcanpool/manual、qxdock/auto、qxribbon/auto）
├── thirdparty/     第三方集成案例（boost / ffmpeg / opencascade / qtitan）
└── projects/       多项目模板（template、staticlink）
```

### 1.3 组件现状矩阵

| 组件           | 版本     | 规模              | 来源 / 定位                          | 状态                         |
| :----------- | :----- | :-------------- | :------------------------------- | :------------------------- |
| **qcanpool** | 2.0.2  | 57 文件 / 8.3k 行  | 自研 legacy 控件（fancy*）+ 简易 ribbon  | ⚠️ 新旧 ribbon 并存，需收敛        |
| **qxribbon** | 0.10.1 | 70 文件 / 15.4k 行 | 自研现代 Ribbon（Office/WPS/Dark 主题）  | ✅ 主力演进中                    |
| **qxdock**   | 0.2.0  | 57 文件 / 16.7k 行 | Qt-Advanced-Docking-System 4.3.1 | ✅ 已同步，较稳定                  |
| **qxwindow** | 0.1.2  | 30 文件 / 5.7k 行  | qwindowkit 派生（原生无边框/系统特性）        | ✅ 稳定                       |
| **qxwidget** | 0.0.1  | 3 文件 / 0.27k 行  | 应用框架整合层                          | 🚧 刚起步，仅 `RibbonAppWindow` |
| 全仓库 src      | —      | ~46.4k 行        | —                                | —                          |

依赖关系：`qxwidget → (qxribbon, qxwindow)`；`qcanpool / qxribbon / qxdock / qxwindow` 相互独立（均只依赖 Qt）。

### 1.4 构建与工具链现状

| 维度      | 现状                                        | 问题                 |
| :------ | :---------------------------------------- | :----------------- |
| 构建系统    | qmake（.pro/.pri）+ CMake 双轨                | 双轨维护成本高            |
| C++ 标准  | qmake 侧 `c++14`，CMake 侧 `C++17`           | 不一致                |
| Qt 版本   | 5.12.12 / 5.14.2 / 5.15.2 / 6.5.3 / 6.8.1 | 兼容面过宽，测试矩阵难以覆盖     |
| 编译器     | MinGW / MSVC2017 / MSVC2022 / GCC         | 旧编译器（MSVC2017）拖累   |
| 包管理     | 无（源码引用 `qtconfig.pri` / `find_package`）   | 无 vcpkg / Conan 集成 |
| CI / 测试 | 无可见 CI；测试仅 2 组 auto                       | 质量门禁缺失             |

### 1.5 生态资产（已有优势，3.0 要放大）

- **示例极其丰富**：`examples/` 细粒度控件示例 + `demos/` 完整应用演示，是同类库少见的。
- **真实落地案例**：MyCAD（基于 FreeCAD 1.0 + QxRibbon），验证了 Ribbon 的工程可用性。
- **多项目模板**：`projects/template` + `qtconfig.pri`，支持"一仓多项目"。
- **第三方集成范式**：`thirdparty/` 展示了 Boost/FFmpeg/OCC/qtitan 的接入方式。

### 1.6 版本演进脉络

```
1.x  合并 qcanpool-1.1.0 + qlite-1.1.0
  ↓
2.0.0  Qt5/Qt6 统一；qmake + cmake；引入 qxribbon / qxframeless / qgood
  ↓
2.2.x  大规模裁剪：移除 qads / qsseditor / qtqrcode / qribbon / qxmaterial /
      qcustomplot / qxframeless / qgood；引入 qxdock / qxwindow
  ↓
2.3.0  确立"只维护核心库"策略；新增 qxwidget（RibbonAppWindow）＝ 3.0 的雏形
  ↓
3.0.0  ← 本规划目标：现代化收敛
```

**结论**：2.x 已经完成了"**减法**"（去冗余、去第三方包袱）与"**组件化拆分**"（qxdock/qxwindow/qxwidget 独立）。3.0 应完成"**加法与统一**"——把拆散的能力收敛成一致、现代、可发布的组件栈与工程体系。

---

## 二、问题与债务诊断

| 编号  | 问题                                                                      | 维度     | 优先级    |
| :-- | :---------------------------------------------------------------------- | :----- | :----- |
| D1  | **两套 Ribbon 并存**：`qcanpool` 内简易 ribbon 与 `qxribbon` 功能重叠                | 架构     | **P0** |
| D2  | **命名体系不统一**：`qcanpool`（QCanpool ns）与 `qx*`（QxRibbon/QxWindow... ns）风格割裂 | 架构/API | **P0** |
| D3  | **`qxwidget` 名不符实**：名为 widget 库，实为应用框架整合层，且仅 1 个类                       | 架构     | **P0** |
| D4  | **构建双轨**：qmake + CMake 并行维护，C++ 标准不一致                                   | 构建     | **P0** |
| D5  | **兼容面过宽**：Qt 5.12~6.8、MSVC2017 全都要支持                                    | 构建     | **P1** |
| D6  | **无 CI / 质量门禁**：无自动构建矩阵、无静态分析、无覆盖率                                      | 质量     | **P1** |
| D7  | **测试覆盖薄**：仅 2 组 auto test，核心控件基本无单测                                     | 质量     | **P1** |
| D8  | **文档薄弱**：无 API 文档、无在线文档、无迁移指南                                           | 文档     | **P1** |
| D9  | **插件体系未落地**：`src/plugins` 仅骨架，`modules` 为空                              | 架构     | **P2** |
| D10 | **版本号割裂**：仓库版本 2.3.0 与各库版本（2.0.2/0.10.1/0.2.0/0.1.2/0.0.1）各自为政，用户难感知    | 发布     | **P2** |
| D11 | **主题系统分散**：样式散落在 qss 资源与各库中，未抽象为统一主题引擎                                  | 体验     | **P2** |
| D12 | **旧 API 迁移路径缺失**：legacy fancy* / ribbon 用户无平滑升级指引                       | 生态     | **P2** |

---

## 三、3.0 定位与目标

### 3.1 一句话定位

> **把 qtcanpool 从"一个 QWidgets 控件集合"升级为"一套 Qt6 优先、CMake 优先、组件化、可发布的现代桌面 UI 应用框架"。**

### 3.2 目标（Goals）

1. **收敛**：一套 Ribbon、一套命名、一套应用框架，消除历史冗余。
2. **现代**：Qt6 优先、CMake 主构建、C++17 统一、可被 `find_package` 消费。
3. **可靠**：CI 全平台构建 + 自动化测试 + 静态分析门禁。
4. **可用**：完整文档（API + 指南 + 迁移）+ 可在线体验的演示。
5. **可扩展**：插件/模块体系真正可用，支撑大型多模块应用。

### 3.3 非目标（Non-Goals）

- 不追求 QML/Quick 方向（继续聚焦 QtWidgets）。
- 不再新增重型第三方依赖（保持核心库轻量、可独立发布）。
- 不保证与 1.x API 二进制兼容（2.x→3.x 提供源码级迁移指引即可）。

---

## 四、3.0 技术方向（六大方向）

### 方向 A：组件收敛与命名统一（P0）

**目标结构**：

```
src/libs/
├── qxcore/      (新) 基础设施：日志 / 设置 / 主题引擎 / i18n / 工具
├── qxwindow/    无边框窗口 / 原生特性
├── qxribbon/    Ribbon 控件（唯一 ribbon 实现）
├── qxdock/      停靠系统
├── qxapp/       (原 qxwidget 升级) 应用框架：AppShell = Window + Ribbon + Dock + Theme
└── qcanpool/    冻结为 legacy 兼容层，标记 deprecated，仅修 bug
```

- **A1**：在 3.0 中**冻结 `qcanpool` 的 legacy ribbon**（`ribbonbar/ribbonpage/ribbongroup/ribbonwindow/ribboncontainers`），文档标注"将被 qxribbon 取代"，2.x 内先加 `QCANPOOL_DEPRECATED_SINCE`。
- **A2**：**统一命名空间与前缀**为 `qx*`（`QxRibbon/QxWindow/QxDock/QxApp`），`qcanpool` 保留原名但冻结。
- **A3**：将 `qxwidget` 重命名/重定位为 **应用框架库**（建议 `qxapp`）：承载 `RibbonAppWindow`、主题切换、设置、状态栏、启动流程等。
- **A4**：`qcanpool` 的 fancy* 控件做"存废评估"：通用性强者迁入 `qxwidget/qxapp` 或独立 `qxfancy`，弱者在 3.x 内下线。

### 方向 B：构建与工具链现代化（P0）

- **B1 CMake 主构建**：以 CMake 为一等公民，完善 `install` / `export` / `find_package(qxribbon)` 等模块化包；
- **B2 qmake 退役**：3.0 冻结 qmake 支持（保留只读兼容），后续版本移除；
- **B3 C++17 统一**：qmake 侧同步到 c++17，去除 14/17 分裂；
- **B4 Qt 目标收敛**：主目标 **Qt 6.5 LTS / 6.8 LTS**；**Qt 5.15 尽力兼容**；**放弃 ≤5.14 与 MSVC2017**；
- **B5 包管理**：提供 **vcpkg / Conan** 描述，降低接入门槛；
- **B6 安装与部署**：统一 `QX_` CMake 命名空间，产出可被下游 `find_package` 直接消费的 SDK。

### 方向 C：质量工程（P1）

- **C1 CI 矩阵**：GitHub Actions + Gitee Go，覆盖 {Win/MinGW, Win/MSVC, Linux/GCC, macOS/Clang} × {Qt 5.15, 6.8}；
- **C2 测试体系**：QTest 为主，核心库（qxribbon/qxwindow/qxdock 的布局/状态/几何逻辑）补单测；demo 做**截图回归**（golden image）；
- **C3 静态分析**：clang-tidy + ASan/UBSan + 覆盖率报告；
- **C4 代码规范自动化**：clang-format 配置入库，PR 强制格式检查；
- **C5 依赖与安全**：第三方来源/许可（`LICENSE.NOTES.md`）自动核对。

### 方向 D：文档与生态（P1）

- **D1 API 文档**：Doxygen 生成 + 在线站点（GitHub Pages / Gitee Pages）；
- **D2 指南体系**：快速开始、架构说明、各组件使用指南、主题定制指南；
- **D3 迁移指南**：`2.x → 3.0` 迁移手册（legacy ribbon → qxribbon 的类/方法对照表）；
- **D4 演示在线化**：将 demos 以 **Qt for WebAssembly** 发布为可点击体验页（品牌曝光利器）；
- **D5 模板与脚手架**：`projects/template` 升级为 3.0 模板 + 一键脚本生成应用骨架。

### 方向 E：新组件与能力（P2，3.0 后续小版本）

| 组件                | 说明                                           |
| :---------------- | :------------------------------------------- |
| **主题引擎**          | 统一 Office/WPS/Dark 主题，支持运行时切换、QSS 集中管理、高 DPI |
| **配置框架**          | QSettings 封装、分组/默认值/迁移                       |
| **日志框架**          | 分级、文件滚动、异步、可挂 UI                             |
| **通知/Toast**      | 应用内轻提示                                       |
| **属性编辑器 / 设置对话框** | 通用配置界面组件                                     |
| **AppShell 骨架**   | 导航 + Ribbon + Dock + 状态栏 + 启动屏 的完整应用外壳       |
| **i18n 基础设施**     | 翻译加载/切换/语言包                                  |

### 方向 F：插件/模块体系落地（P2）

- **F1**：让 `src/plugins` 从骨架变为可用（插件元数据、动态加载、依赖解析、管理器 UI）；
- **F2**：`src/modules` 落地为"可选业务模块"层（如：文件浏览器、属性面板、输出面板）；
- **F3**：以 **IDE 式应用**（MyCAD 类场景）作为插件体系的首个样板。

---

## 五、分阶段路线图

| 阶段        | 目标        | 关键交付                                                   | 出口标准                               |
| :-------- | :-------- | :----------------------------------------------------- | :--------------------------------- |
| **M1 地基** | 统一构建与质量底座 | CMake 主构建成型、C++17 统一、CI 矩阵跑通、clang-format、单测骨架         | 全平台 CI 绿灯，`find_package` 可消费       |
| **M2 收敛** | 消除冗余、统一命名 | 冻结 legacy ribbon、统一 `qx*` 命名、`qxwidget→qxapp` 重定位、迁移指南 | 无重复 ribbon；旧 API 全部有 deprecated 标注 |
| **M3 增强** | 新能力与体验    | 主题引擎、配置/日志、AppShell、文档站点、在线 demo                       | 文档可发布；demo 可在线体验                   |
| **M4 发布** | 3.0.0 正式版 | 版本对齐、Release Notes、发布包（含 vcpkg/Conan）                  | 3.0.0 tag + 发布说明 + 迁移指引            |

> 节奏建议：**M1/M2 为核心投入**（决定 3.0 的"骨架"），M3 可按小版本增量推进，M4 收口发布。
>
> **进度（2026-10-10）**
>
> - ✅ **M1 地基已完成**：[`design/3.0-M1-TASKS.md`](./design/3.0-M1-TASKS.md)
> - ✅ **M2 收敛核心项已落地**：`qxwidget→qxapp` 重定位、legacy ribbon deprecated 标注：[`design/3.0-M2-TASKS.md`](./design/3.0-M2-TASKS.md)
> - ✅ **M3.1 qxcore 基础设施已落地**：新增 `src/libs/qxcore`（`QxSettings` 配置 + `QxLogger` 日志），并修复测试驱动吞掉失败的问题：[`design/3.0-M3-TASKS.md`](./design/3.0-M3-TASKS.md)
> - ✅ **M3.2 主题引擎已落地**：新增 `src/libs/qxtheme`（`QxThemeManager` 统一应用调色板 + 样式表、运行时切换、选择持久化、跟随系统深浅色）；与 qxribbon 解耦，不改变既有库依赖
> - ✅ **M3.3 AppShell 骨架已落地**：`qxapp` 内新增 `QxAppShell`（导航 + 页面栈 + DockWindow + 状态栏 + 布局持久化）、`QxNavigationBar`、`QxSplashScreen`，并带 CMake demo `demos/qxapp/appshell`；qmake 侧 `qxapp` 描述文件未动
> - ✅ **M3.4 文档站点已上线**：`doc/` 成为可独立构建的 Doxygen 工程（`cmake -S doc -B build-docs`）——API 取自 `src/libs` 头文件、指南取自 `doc/pages`，产物 966 页；新增 `.github/workflows/docs.yml` 发布到 <https://canpool.github.io/qtcanpool/>（`release-*` 分支）。同时修复了 M3.3 的 `autoSaveOnClose` 用例在 Linux + Qt 6.8 上的失败
> - ✅ **legacy 库 Doxygen 警告清零**：55 条 → **0**。其中有 8 条是真坏掉的注释块（`fancytitlebar.cpp` 里未闭合的反引号把文档吞到文件尾、`dockoverlay.cpp` 的 `\code` 缺 `\endcode`），另 16 条来自 `qxwindow` 平台后端的孤儿注释（`Q_OS_MAC` 预定义 + 排除 Windows 后端后归零），其余是 `@param` / `@ref` 与签名对不上
> - ✅ **M3.5 在线 demo 已上线**：`AppShellDemo` 编译为 **WebAssembly**，随文档站点一同发布到 <https://canpool.github.io/qtcanpool/demo/>（浏览器打开即用，无需 Qt / 编译器）。为此新增 `QTCANPOOL_WASM` 平台开关、`QX_DOCK_X11` 覆盖 wasm 上"`Q_OS_UNIX` 与 `Q_OS_WASM` 同时定义"的陷阱，并把 `.github/workflows/docs.yml` 改名 `pages.yml` 增加 `wasm` 作业
> - ✅ **M4.1 SDK 可消费性已落地**：`find_package(QtCanpool)` 的「安装 → 消费」闭环。为此修复两个从未被发现的导出缺陷（配置里多余的 `Concurrent`/`Core5Compat` 依赖、`qxribbon_global.h` 未随包安装），并新增下游工程 `projects/consume` 与 CI 门禁（`ubuntu-latest / Qt 6.8.1` 上跑完整往返）
> - ✅ **M4.2 ~ M4.4 已落地**：项目版本对齐 `3.0.0`；`CHANGELOG` 补 `3.0.0` 条目；新增 vcpkg / Conan 骨架（`ports/` + `conanfile.py`，未经 CI 验证）
> - ⏸ **M4.5 打 tag（`3.0.0`）待作者确认**：前置条件已全部就绪（CI 全绿、发布说明成稿、SDK 可消费）
> - ⏸ M2 的 legacy ribbon **物理下线**按 A1 顺延至 3.x

---

## 六、关键决策（已确认 ✅）

> 决策人：项目作者 · 确认日期：2026-10-08 · 以下决策为 3.0 的基线约束，后续如需变更须走决策记录。

| #      | 决策                 | 最终结论                                                          | 落地影响                                 |
| :----- | :----------------- | :------------------------------------------------------------ | :----------------------------------- |
| **K1** | Qt 最低支持版本          | ✅ **Qt 5.15 尽力兼容，主推 Qt 6.5 / 6.8 LTS**                        | 放弃 ≤5.14 与 MSVC2017；CI 覆盖 5.15 + 6.8 |
| **K2** | qmake 去留           | ✅ **3.0 冻结（只读），后续版本移除**                                       | 新增内容一律只进 CMake；.pro 不再新增特性           |
| **K3** | 统一命名前缀             | ✅ **统一为 `qx*`（QxRibbon/QxWindow/QxDock/QxApp），`qcanpool` 冻结** | 库/include/宏/命名空间统一风格                 |
| **K4** | `qxwidget` 去向      | ✅ **重定位为 `qxapp`（应用框架）**                                      | 命名空间 QxWidget→QxApp，保留转发兼容期          |
| **K5** | legacy `fancy*` 控件 | ✅ **评估后：通用者迁入 qxapp/qxcore，专用者 3.x 内下线**                      | 见《3.0-MIGRATION》存废表                  |
| **K6** | 源码/二进制兼容           | ✅ **不保证二进制兼容，提供 2.x→3.0 源码级迁移指南**                             | 旧 API 加 deprecated 宏 + 迁移对照表         |

### 配套设计文档

| 文档                                                         | 内容                                                                   |
| :--------------------------------------------------------- | :------------------------------------------------------------------- |
| [`doc/design/3.0-MIGRATION.md`](./design/3.0-MIGRATION.md) | 2.x→3.0 迁移指南：命名空间、Ribbon 类/方法对照、fancy* 存废、qxwidget→qxapp、构建迁移        |
| [`doc/design/3.0-M1-TASKS.md`](./design/3.0-M1-TASKS.md)   | M1 可执行任务清单：CMake 主构建、C++17、CI 矩阵、clang-format、测试骨架（含开箱即用配置）          |
| [`doc/design/3.0-M2-TASKS.md`](./design/3.0-M2-TASKS.md)   | M2 可执行任务清单：`qxwidget→qxapp` 重命名、legacy ribbon deprecated 标注（含实测踩坑记录） |
| [`doc/design/3.0-M3-TASKS.md`](./design/3.0-M3-TASKS.md)   | M3 增量清单 + 执行记录：`qxcore` 配置/日志库、测试退出码修复、`qxtheme` 主题引擎、`qxapp` AppShell 骨架、Doxygen 文档站点与 Pages 发布（含探针与踩坑） |
| [`doc/design/3.0-M4-TASKS.md`](./design/3.0-M4-TASKS.md)   | M4 发布清单 + 执行记录：SDK 可消费性（含两个导出缺陷的修复）、版本对齐、发布说明、vcpkg/Conan 骨架、打 tag 的暂停点 |

---

## 七、风险与依赖

| 风险                         | 影响 | 缓解                         |
| :------------------------- | :- | :------------------------- |
| 收敛幅度大，破坏存量用户（如 MyCAD）      | 高  | 保留 legacy 冻结库 + 迁移指南；给足过渡期 |
| 个人维护带宽有限                   | 高  | 按里程碑小步走；优先自动化（CI/测试/文档）减负  |
| Qt6 WebAssembly 打包 demo 有坑 | 中  | ✅ 已解决：锁定 Qt 6.8.3 + Emscripten 3.1.56 组合、强制静态库（`QTC_STATIC_BUILD`）|
| vcpkg/Conan 维护成本           | 中  | 先出最简 port，社区可贡献            |
| 命名/更名引发下游引用断裂              | 中  | 提供兼容头 + 过渡宏                |

---

## 八、验收标准（Definition of Done for 3.0）

1. `src/libs` 中**只有一套 Ribbon 实现**，命名空间前缀统一；
2. 主构建为 **CMake**，C++17，可在 Qt 6.5/6.8 + MSVC2022/MinGW/GCC 上构建；
3. **CI 全平台绿灯**，核心库具备自动化测试与覆盖率报告；
4. 发布 **在线 API 文档 + 组件指南 + 2.x→3.0 迁移指南**；
5. demos 至少 **1 个可在线体验**（WebAssembly）✅ `AppShellDemo` → <https://canpool.github.io/qtcanpool/demo/>；
6. 产出可被 `find_package` 消费的 **SDK 包**（+ vcpkg/Conan port）；
7. 版本号对齐并打 tag，附 Release Notes。

---

## 附：3.0 目标库结构（Before → After）

```
Before (2.3.0)                      After (3.0)
─────────────────────────────       ─────────────────────────────
src/libs/qcanpool  (含简易 ribbon)   src/libs/qcanpool   (legacy, 冻结)
src/libs/qxribbon                   src/libs/qxribbon   (唯一 ribbon)
src/libs/qxdock                     src/libs/qxdock
src/libs/qxwindow                   src/libs/qxwindow
src/libs/qxwidget  (1 个类)          src/libs/qxapp      (应用框架)
                                     src/libs/qxcore     (基础设施, 新增)
```


```

---

## 附录 B：CHANGELOG 3.0.0 草案

> 说明：以下为发布前草案，最终以正式发布说明为准。

```

## 3.0.0

- qt: Qt6-first, primary support Qt 6.5 LTS / 6.8 LTS; best-effort Qt 5.15
- qt: drop Qt <= 5.14 and MSVC2017
- project: cmake is the primary build system; qmake frozen (read-only)
- project: unify C++ standard to C++17
- project: unify library naming to qx* namespace (QxRibbon/QxWindow/QxDock/QxApp)
- project: provide installable CMake packages (find_package(QxRibbon) etc.)
- qcanpool: freeze legacy ribbon (ribbonbar/ribbonpage/ribbongroup/ribbonwindow)
- qcanpool: mark legacy API deprecated, add migration guide
- qxribbon: the only ribbon implementation; release version x.y.z
- qxwidget: renamed/relocated to qxapp (application framework)
- qxapp: AppShell combining Window + Ribbon + Dock + Theme
- qxcore: new infrastructure library (logging/config/theme/i18n)
- ci: add cross-platform build matrix (Windows/Linux/macOS x Qt5.15/Qt6.8)
- test: expand automated tests and enable coverage report
- docs: add API reference, component guides and 2.x -> 3.0 migration guide

```
```
