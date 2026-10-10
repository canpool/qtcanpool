# qtcanpool 3.x 开发规划

> 上承：[`ROADMAP-3.0.md`](./ROADMAP-3.0.md)（3.0「现代化收敛」，已发布 `3.0.0`）
> 版本定位：**欠账清算与能力补齐（Pay Down & Fill In）**
> 主题：**legacy 收敛收口 · 质量门禁补齐 · 应用框架可用化 · 插件体系落地**
> 文档状态：**3.1.0 已发布**（2026-10-10）· **3.2.0 已发布**（2026-10-10），任务清单见 [`design/3.2-TASKS.md`](./design/3.2-TASKS.md)
> 适用仓库：`https://github.com/canpool/qtcanpool` / `https://gitee.com/icanpool/qtcanpool`

---

## 一、3.0 交付复盘（下一步的出发点）

### 1.1 3.0 到底交付了什么

3.0 不是"只做了架构调整"，它同时从零建起了工程化地基。但三层交付的**分量极不均衡**：

| 层面     | 3.0 交付                                                                          | 性质      |
| :----- | :------------------------------------------------------------------------------ | :------ |
| 工程地基   | CMake 主构建、CI 四平台矩阵、clang-format 强制门禁、可被 `find_package` 消费的 SDK、Doxygen 文档站点、在线 WebAssembly demo、vcpkg/Conan 骨架 | **全新**  |
| 架构收敛   | `qx*` 命名统一、`qxwidget → qxapp` 重定位、拆出 `qxcore` / `qxtheme`、`qcanpool` 冻结并标 deprecated | **结构调整** |
| 组件能力   | 仅 4 件：`QxSettings`、`QxLogger`、`QxThemeManager`、`QxAppShell`（+ `QxNavigationBar`、`QxSplashScreen`） | **增量很窄** |

### 1.2 功能零改动的部分

三个主力组件库在 3.0 中**功能代码一行未改**（只改了构建脚本、命名空间与跨平台守卫）：

| 库          | 规模                    | 3.0 中的改动                      |
| :--------- | :-------------------- | :---------------------------- |
| `qxribbon` | 41 头 / 29 源（约 15.4k 行） | 仅 CMake 与安装导出（补 `qxribbon_global.h` 等） |
| `qxdock`   | 29 头 / 26 源（约 16.7k 行） | 仅 CMake 与 `QX_DOCK_X11` 平台守卫     |
| `qxwindow` | 14 头 / 11 源（约 5.7k 行）  | 仅 CMake 与 WASM 后端开关            |

> **结论**：3.0 换的是"地基与骨架"，血肉几乎没长。因此 3.x 的核心命题不是"再加几个控件"，而是**先把手上的债还清、再让骨架真正能承重**。

### 1.3 3.0 明确留下、且已承诺要完成的欠账

以下均有文档依据（路线图 / 迁移指南白纸黑字写明"3.x 内执行"），不是推测：

| 编号      | 欠账                                                                                              | 依据                             |
| :------ | :---------------------------------------------------------------------------------------------- | :----------------------------- |
| **D13** | **legacy ribbon 物理上仍是两套**。3.0 的 DoD 第 1 条要求"`src/libs` 中只有一套 Ribbon 实现"，实际只做到了"标记 deprecated"，`qcanpool` 内那套仍在 | `ROADMAP-3.0.md` 八·1            |
| **D14** | **K5 存废表未执行**。仅 ribbon 家族（`RibbonBar`/`RibbonPage`/`RibbonGroup`/`RibbonWindow`/`RibbonContainer*`）标了 `QCANPOOL_DEPRECATED_X`；18 个 `fancy*` / `tiny*` / `mini*` / `menu*` / `window*` 头**一个字没标** | `3.0-MIGRATION.md` §3、K5      |
| **D15** | **无覆盖率报告**（CMake 与 CI 中均无 `coverage` / `gcov` / `lcov` 配置）                                          | `ROADMAP-3.0.md` C3            |
| **D16** | **无静态分析**：无 clang-tidy；`WITH_SANITIZE` 选项在 CMake 里存在但 CI 从不启用                                    | `ROADMAP-3.0.md` C3            |
| **D17** | **`qxwindow` 无自动化测试**（`tests/` 下有 `qxcore`/`qxtheme`/`qxapp`/`qxribbon`/`qxdock`，独缺 `qxwindow`） | `ROADMAP-3.0.md` C2、D7         |
| **D18** | **无 demo 截图回归**（golden image）                                                                    | `ROADMAP-3.0.md` C2            |
| **D19** | **`projects/template` 仍是 qmake**（`template.pro` + `config.pri`），无脚手架脚本                               | `ROADMAP-3.0.md` D5            |
| **D20** | **无 i18n 基础设施**（`qxcore` 只有 Settings/Logger，框架无翻译加载/语言切换能力）                                        | `ROADMAP-3.0.md` 方向 E          |
| **D21** | **无 Toast / 通知、无通用设置对话框、无属性编辑器**                                                                 | `ROADMAP-3.0.md` 方向 E          |
| **D22** | **`src/plugins` 仅骨架、`src/modules` 为空目录**，插件体系完全未启动                                                 | `ROADMAP-3.0.md` 方向 F、D9       |
| **D23** | **各库版本号与项目版本解耦**（`qxcore` 0.1.0 / `qxtheme` 0.1.0 / `qxdock` 0.1.0 / `qxribbon` 0.10.1 / `qxwindow` 0.1.2 / `qxapp` 0.0.1），用户难感知 | `ROADMAP-3.0.md` D10           |

### 1.4 迁移清单的进度

`3.0-MIGRATION.md` 的 Step 1-12（下游用户的迁移动作）**全部未勾选** —— 那是**给用户**的清单，属正常。但其中 Step 11「清理 `fancy*` 遗留调用」依赖 D14 的落地，即**库侧先要把存废做完，用户侧才有明确目标**。

---

## 二、问题与债务诊断

| 编号      | 问题                                          | 维度  | 优先级    |
| :------ | :------------------------------------------ | :-- | :----- |
| D13     | 两套 Ribbon 物理并存，3.0 收敛目标未闭环                   | 架构  | **P0** |
| D14     | `fancy*` 等 18 个遗留头未标注、未迁移、未下线                | 架构  | **P0** |
| D15/D16 | 无覆盖率、无静态分析、消毒器未启用                           | 质量  | **P0** |
| D17/D18 | `qxwindow` 无单测、无截图回归                         | 质量  | **P1** |
| D19     | 工程模板停留在 qmake，3.0 用户无脚手架可用                   | 生态  | **P1** |
| D20/D21 | 框架缺 i18n / Toast / 设置界面等"真实应用必备件"             | 能力  | **P1** |
| D22     | 插件与模块体系空白                                   | 架构  | **P2** |
| D23     | 版本号体系割裂                                     | 发布  | **P2** |

---

## 三、定位与目标

### 3.1 一句话定位

> **先把 3.0 的承诺兑现干净（一套 Ribbon、一套门禁），再把 `QxAppShell` 从"能跑的 demo"变成"能起真实项目的框架"，最后在其上长出插件与模块体系。**

### 3.2 目标（Goals）

1. **收口**：`src/libs` 内 legacy ribbon 与 `fancy*` 按下线表处置完毕，真正只剩一套 Ribbon。
2. **可信**：核心库具备覆盖率报告、静态分析与消毒器门禁；`qxwindow` 补齐单测。
3. **可用**：`projects/template` 升级为 3.x CMake 模板 + 脚手架，新建应用一条命令起手。
4. **可做真应用**：`qxcore`/`qxapp` 提供 i18n、通知、设置界面等常用件。
5. **可扩展**：插件/模块体系从骨架变为可用，有 IDE 式样板验证。

### 3.3 非目标（Non-Goals）

- 仍不做 QML/Quick（延续 3.0）。
- 不引入重型第三方依赖。
- 不为 3.0 的用户提供二进制兼容（延续 K6，走源码级迁移）。
- **不为"加控件"而加控件**——3.2 的能力项必须来自"真实应用反复要写"的清单。

---

## 四、技术方向

### 阶段一（3.1）：偿还欠账

#### 方向 A：legacy 收敛收口（P0）

**A1 迁移（先迁后用）** —— 按 `3.0-MIGRATION.md` §3 存废表，把通用控件迁入 `qxapp`/`qxcore`：

| 来源                                                                       | 去向                        |
| :----------------------------------------------------------------------- | :------------------------ |
| `FancyWindow`、`FancyDialog`                                             | `qxapp`（应用窗口 / 通用对话框）     |
| `FancyToolButton`、`ExtensionButton`、`MenuButton`、`MenuAccessButton`      | `qxapp`（通用小控件与按钮外观）       |
| `TinyTabBar`、`TinyTabWidget`、`TinyNavBar`                              | `qxapp`（轻量 Tab / 导航）      |

**A2 下线** —— 与 `qx*` 重叠且无独立价值的，标 deprecated 后在 3.1 内移除：

| 控件                                                       | 理由                     |
| :------------------------------------------------------- | :--------------------- |
| `QuickAccessBar`                                         | 已被 `RibbonQuickAccessBar` 取代 |
| `FancyTitleBar`、`WindowLogo`、`WindowToolBar`             | 与 `qxwindow` / `qxribbon` 窗口方案重叠 |
| `MiniTabBar`、`MiniTabWidget`                            | 与 `Tiny*` 系列重叠         |

**A3 物理移除 legacy ribbon** —— `RibbonBar` / `RibbonPage` / `RibbonGroup` / `RibbonWindow` / `RibbonContainer*` 在 3.1 内从 `qcanpool` **删除**，仅保留迁移文档对照表。这一步完成后 **D13 才真正闭环**（DoD #1 达成）。

**A4 补全 deprecated 标注** —— 对暂不下线的遗留头补齐 `QCANPOOL_DEPRECATED_X`，让"编译告警即迁移点"（迁移文档 Step 12）成立。

> ⚠️ **破坏性提醒**：A3 会破坏 `qcanpool` 的源码兼容。需在 `CHANGELOG` 里显著标注，并确认存量用户（如 MyCAD）已完成迁移——这是本阶段**唯一需要向作者确认的破坏性动作**。

#### 方向 B：质量门禁补齐（P0）

- **B1 覆盖率**：CMake 加 `WITH_COVERAGE`，CI 的 `ubuntu-latest / Qt 6.8.1` 腿产出 lcov 报告并上传为 artifact。**先出报告不设硬阈值**，稳定后再议是否设门禁。
- **B2 静态分析与消毒器**：新增 clang-tidy 作业（先只跑 `src/libs`，`--warnings-as-errors` 从"仅新增告警"起步）；把已有的 `WITH_SANITIZE` 用起来，在 CI 增加 ASan/UBSan 的 Linux 构建腿。
- **B3 `qxwindow` 单测**：新增 `tests/qxwindow/auto/`，优先覆盖**平台无关逻辑**（窗口上下文选择、尺寸/命中计算、命中几何），原生后端只做"能构造"级别验证。
- **B4 demo 截图回归（可选）**：`QT_QPA_PLATFORM=offscreen` 下对 `RibbonDemo`/`DockDemo` 截图比对。收益高但易碎，建议放在本阶段**末尾且允许失败不阻塞**。（3.1 实际落地只做了 `RibbonDemo`：demo 自带 `--screenshot`，sanity 层每次跑、金样层显式选择加入，见 [`design/3.1-TASKS.md`](./design/3.1-TASKS.md) §四）

#### 方向 C：工程模板与脚手架（P1）

- **C1 `projects/template` CMake 化**：给出 3.x 的最小应用模板（`find_package(QtCanpool)` + `QxAppShell` + `main.cpp`），保留 qmake 版本作为 legacy 参考。
- **C2 脚手架脚本**：`scripts/new-project`（Python 3）一键生成"应用骨架"，输入项目名即产出可编译工程。
- **C3 CMake Presets**：`CMakePresets.json` 固化本仓的常用配置（Qt6 MinGW/msvc、Qt5.15、WASM、覆盖率、消毒器），降低本机与 CI 的配置漂移。

### 阶段二（3.2 / 3.3）：应用框架可用化

#### 方向 D：让 `QxAppShell` 能承重（P1）

按"真实应用反复要写"排序。本阶段**拆成两个小版本增量发布**（K12）：3.2 交付"必备三件 + 内核"，
其余后移 3.3。

| 能力                | 落点       | 说明                                                         | 版本     |
| :---------------- | :------- | :--------------------------------------------------------- | :----- |
| **i18n 基础设施**     | `qxcore` | 翻译加载/切换/语言包枚举、运行时重翻译（`QEvent::LanguageChange` 广播）           | **3.2** |
| **通知 / Toast**    | `qxapp`  | 应用内轻提示：级别、自动消失、可堆叠、可挂到 AppShell                            | **3.2** |
| **属性编辑器**         | `qxapp`  | 与设置对话框共用的键值编辑内核                                            | **3.2** |
| **通用设置对话框**       | `qxapp`  | 分组 + 表单 + 应用/取消语义，对接 `QxSettings`                           | **3.2** |
| **工作区 / 布局预设**    | `qxapp`  | 在现有布局持久化之上加"命名工作区"与预设切换（`saveLayout()` 已可用，属叠加便利性，非缺口）     | 3.3    |
| **状态栏 / 进度 / 忙指示** | `qxapp`  | `QxAppShell` 已有 `setStatusMessage()` + `setBusy()`，只缺百分比进度条，属小补丁 | 3.3    |

> 每个能力都必须有对应的 `tests/` 单测与文档页，避免重演 3.0"骨架有、测试薄"的问题。
> 任务分解与验收标准见 [`design/3.2-TASKS.md`](./design/3.2-TASKS.md)。

### 阶段三（4.0）：插件与模块体系

#### 方向 E：从"控件库"走向"应用平台"（P2）

- **E1 插件元数据与加载**：插件清单（id / 版本 / 依赖 / 能力）、动态加载（`QPluginLoader`）、失败隔离与诊断。
- **E2 依赖解析与生命周期**：加载顺序求解、延迟加载、启停与注销。
- **E3 插件管理器 UI**：列表 / 启停 / 依赖视图（可放 `qxapp`）。
- **E4 `src/modules` 业务模块层**：文件浏览器、属性面板、输出面板等"可选业务模块"样板。
- **E5 IDE 式样板**：以 MyCAD 类场景验证（对应 3.0 方向 F3）。

> **前置条件**：E 必须在 A（API 干净）、B（有测试与静态分析兜底）、D（有设置/日志/主题基础设施）之后启动——否则是在沙地上盖楼。

---

## 五、分阶段路线图

| 阶段            | 主题        | 关键交付                                                                        | 出口标准                                        |
| :------------ | :-------- | :-------------------------------------------------------------------------- | :------------------------------------------ |
| **M5（3.1）**  | 偿还欠账      | A：legacy 迁移/下线/物理移除 + deprecated 补全；B：覆盖率 + clang-tidy + 消毒器 + `qxwindow` 单测；C：模板 CMake 化 + 脚手架 + Presets | 只剩一套 Ribbon；CI 产出覆盖率与静态分析；`scripts/new-project` 可起新工程 |
| **M6（3.2）**  | 能力增量      | D 第一批：i18n + Toast + 属性编辑器内核 + 设置对话框；**R：`qcanpool` 整库删除**                  | `src/libs` 只剩 6 个库；脚手架起的应用能切语言、弹 Toast、开设置界面；每项有单测与文档 |
| **M6b（3.3）** | 能力增量（续）   | D 第二批：工作区/布局预设、状态栏进度                                                    | 应用可存/取命名工作区；状态栏有进度指示                          |
| **M7（4.0）**  | 插件与模块     | E：插件加载/依赖/管理器 UI + `src/modules` + IDE 样板                                    | 插件可独立编译与热插拔；样板应用以模块拼装而成                     |

> 节奏建议：**M5 是可独立交付的小版本（3.1），收益立竿见影且零新增 API 面**；M6 按能力项拆小版本增量发布（3.2 / 3.3 …）；M7 才起 4.0 大版本。
>
> **进度（2026-10-10）**
>
> - ✅ **M5 已发布 `3.1.0`**：tag `3.1.0`（2026-10-10）+ [GitHub Release](https://github.com/canpool/qtcanpool/releases/tag/3.1.0)；变更清单见 `CHANGELOG`，升级前先读迁移指南
> - ✅ **K7 已确认**：legacy ribbon 于 3.1 内物理移除
> - ✅ **M5 代码与文档已收敛完毕**：任务清单与执行记录见 [`design/3.1-TASKS.md`](./design/3.1-TASKS.md)，
>   含可选的 B4（demo 截图回归）在内全部落地
>   - ✅ **A3** legacy ribbon 物理移除（`qcanpool` 只剩一套 Ribbon，DoD #1 达成）
>   - ✅ **A1** legacy 通用控件迁入 `qxapp`（7 个类换名入 `QxApp::`，`qcanpool` 留同名转发头一个版本）
>   - ✅ **A2** legacy 专用控件下线（11 个类物理删除，`qcanpool` 不再含翻译单元）
>   - ✅ **A4** `deprecated` 标注（随 A1 完成；已在库外以 `-Werror=deprecated-declarations` 实测可作门禁）
>   - ✅ **B1** 覆盖率报告（`WITH_COVERAGE` + CI `quality` 作业，按 K8 不设阈值）
>   - ✅ **B2** clang-tidy + ASan/UBSan（覆盖率与消毒器在 `quality` 作业（门禁），clang-tidy 独立成
>     `tidy` 作业（只报告）——拆开是因为原先它排在门禁步骤之后，一个不阻塞的步骤反而占着关键路径）
>   - ✅ **B3** `qxwindow` 自动化测试（8 用例，并顺带修掉一处解空指针崩溃）
>   - ✅ **C3** `CMakePresets.json`（6 个配置预设，环境变量可覆盖默认路径）
>   - ✅ **C1** `projects/template` CMake 化（`find_package(QtCanpool)` + `QxAppShell` 最小应用；
>     已在本地「安装 SDK → 配置 → 编译 → 运行」全链路验证）
>   - ✅ **C2** `scripts/new-project` 脚手架（一条命令生成 + 编译可运行的 `QxAppShell` 应用；
>     不新增第二套模板工程，K9）
>   - ✅ **B4** demo 截图回归（`RibbonDemo` 自渲染 `--screenshot`；sanity 层每次跑，金样层
>     `QTCANPOOL_VISUAL_BASELINE=1` 显式选择加入——截图只在生成金样的同一台机器上可比，CI 不判金样）
> - ⚠️ **A 组是三个连续破坏性变更**，升级前务必读 `doc/design/3.0-MIGRATION.md`。要点是
>   A1 与 A2 的**性质不同**：A1 之后旧名仍能编译（只是告警），A2 之后旧名不存在、必须改代码。
> - ⚠️ **B2 已产生第一笔实际收益**：`quality` 作业首次上线即被 UBSan 逐条抓出**两处既存缺陷**
>   （`~DockContainer` 从基类析构回调派生类；`DockSideBar::insertDockWidget` 对空焦点控制器
>   发起成员调用，默认配置下即可命中），均已修复。详见
>   [`design/3.1-TASKS.md`](./design/3.1-TASKS.md) 执行记录。
> - ✅ **M6 已细化并拆版**（2026-10-10）：**3.2** = i18n + Toast + 属性编辑器内核 + 设置对话框，
>   外加 **`qcanpool` 整库删除**（K5 的缓冲期到期，`src/libs` 收敛到 6 个库）；工作区预设、
>   状态栏进度留 **3.3**（K12）。任务清单见 [`design/3.2-TASKS.md`](./design/3.2-TASKS.md)
> - ✅ **M6（3.2）已发布 `3.2.0`**（2026-10-10）：tag `3.2.0` +
>   [GitHub Release](https://github.com/canpool/qtcanpool/releases/tag/3.2.0)；变更清单见 `CHANGELOG`，
>   升级前先读迁移指南。**这一版是破坏性的**：`qcanpool` 整库删除，A1 留的转发头也没了。
>   - ✅ **R1** `qcanpool` 整库删除（`src/libs` 只剩 6 个库）
>   - ✅ **R2** legacy 示例与构建接线清理（含 `projects/template` 的 qmake 侧整份删除）
>   - ✅ **D1** i18n（`QxTranslator`，`qxcore`）
>   - ✅ **D2** Toast 应用内通知（`QxToast` / `QxToastManager`，`qxapp`）
>   - ✅ **D3** 属性编辑器内核（`QxProperty` / `QxPropertyEditor`）
>   - ✅ **D4** 设置对话框（`QxSettingsDialog`）
>   - ✅ **R3** 迁移文档与文档站收尾（含 K11 的「库版本 ↔ 项目版本」对照表）
>   - ✅ 验收标准 #2：`projects/template` 与 `scripts/new-project` 生成的工程
>     已接好切语言、弹 Toast、开设置界面
> - ⏸ **M6b（3.3）**：工作区 / 布局预设、状态栏进度（K12 已定）
> - ⏸ M7（4.0）插件与模块，待 3.2 / 3.3 之后再细化

---

## 六、关键决策

> K7–K12 均已定案。K5（legacy `fancy*` 存废表）出自 [`ROADMAP-3.0.md`](./ROADMAP-3.0.md)。

| #       | 决策             | 结论                                                     | 影响                                  |
| :------ | :------------- | :------------------------------------------------------- | :---------------------------------- |
| **K7**  | legacy ribbon 物理移除时机 | ✅ **3.1 内完成**（已确认 2026-10-10；先迁入 `qxapp`，再删除 `qcanpool` 的 ribbon 家族） | 破坏 `qcanpool` 源码兼容；已确认放行         |
| **K8**  | 覆盖率是否设硬门禁      | **先出报告、不设阈值**；稳定 1~2 个版本后再议                              | 避免为凑数字写无效测试                          |
| **K9**  | 脚手架形态          | **CMake Presets + 脚本生成骨架**（而非维护第二套模板工程）                     | 影响 `projects/template` 与 `scripts/` 的形态 |
| **K10** | 能力项的取舍边界    | **只收"真实应用反复手写"的件**，不做"别人有我们也要有"的功能                        | 防止能力线无限膨胀                            |
| **K11** | 各库版本号是否收敛      | ✅ **保持各库独立演进号**（已确认 2026-10-10）；文档首页给出「库版本 ↔ 项目版本」对照表        | D23 的处置方式已定                        |
| **K12** | 3.2 / 3.3 的拆分     | ✅ **3.2 = i18n + Toast + 属性编辑器内核 + 设置对话框 + `qcanpool` 删除**（已确认 2026-10-10）；工作区预设、状态栏进度留 3.3 | 版本节奏与范围；M6 的 DoD 拆成两批            |

---

## 七、风险与依赖

| 风险                             | 影响 | 缓解                                       |
| :----------------------------- | :- | :--------------------------------------- |
| A 组三次破坏性变更（A3 移除 ribbon、A1 搬迁、A2 删除）冲击存量用户 | 高  | 先确认 MyCAD 等用户已迁移；`CHANGELOG` 逐条显著标注；保留迁移对照表；A1 用"带告警的转发头"给一个版本缓冲 |
| 3.2 删除 `qcanpool` 冲击仍用旧名的下游（转发头消失，**不再编译**） | 中  | 迁移指南显著标注「A1 是告警、R1 是消失」的区别；这是 K5 承诺的到期点，非新增决策 |
| 个人维护带宽有限（延续 3.0 风险）            | 高  | 优先 B（自动化减负）；M5 拆成可独立发布的小版本               |
| 覆盖率/静态分析引入大量噪声                 | 中  | K8 不设阈值；clang-tidy 从"仅新增告警"起步            |
| 截图回归易碎                         | 中  | 置于阶段末尾，允许失败不阻塞                           |
| 3.2 能力项无限膨胀                    | 中  | K10 约束来源；每项必须有对应单测与文档页                   |
| 插件体系过早启动                       | 中  | 明确 E 的前置条件是 A/B/D 完成                      |

---

## 八、验收标准

### M5 · 3.1「偿还欠账」

1. ✅ `src/libs` 中**物理上只有一套 Ribbon 实现**（legacy ribbon 已移除）——3.0 DoD #1 真正达成；
2. ✅ `fancy*` 等遗留头**全部**按存废表处置完毕（迁移或下线），无"标了 deprecated 却永不处理"的悬案
   —— 7 个迁入 `qxapp` 并保留**带告警**的转发头（3.2 删除），11 个直接删除；库内不再有任何未标注的遗留头；
3. ✅ CI 产出**覆盖率报告**与 **clang-tidy 结果**，并有至少一条 **ASan/UBSan** 构建腿（首跑即抓到两处真实 UB）；
4. ✅ `qxwindow` 有自动化测试且纳入 CI；
5. ✅ `scripts/new-project` 能一条命令生成并编译出可运行的 `QxAppShell` 应用（CI 同腿实测）；
6. ✅ `CMakePresets.json` 覆盖本仓常用配置（Qt6、Qt5.15、WASM、覆盖率、消毒器）；
7. ✅ 文档站点同步更新（迁移表、构建指南、新增的质量门禁说明）——迁移表已按 A1/A2 落定结果重写，
   含每个被删类的替代对照与「A1 与 A2 性质不同」的显式提醒。

### M6 · 3.2「能力增量（第一批）」

8. 用脚手架生成的应用**开箱**即可切换语言、弹出 Toast、打开设置对话框；
9. 上述每项能力均有单测（在 CI 门禁内跑）与对应文档页；
10. `qcanpool` 库**物理消失**：`src/libs` 只剩 6 个库，全树无 `qcanpool/` 引用；
11. 不新增任何"没有真实使用场景"的控件。

### M6b · 3.3「能力增量（第二批）」

12. 应用可保存 / 恢复**命名工作区**，并在预设间切换；
13. 状态栏提供**百分比进度指示**（与已有消息行、忙指示并列）。

### M7 · 4.0「插件与模块」

14. 插件可独立编译、按元数据加载、依赖可解析、失败可隔离；
15. 存在一个**由模块拼装**、可运行的 IDE 式样板应用；
16. 插件接口有版本化约定与迁移说明。

---

## 附：3.0 → 3.x 目标结构（Before → After）

```
             3.0.0                        3.1.0（已发布）                    3.2.0（已发布）
──────────────────────────────  ───────────────────────────────  ──────────────────────────────
qcanpool (legacy 冻结)          qcanpool (7 个转发头，无翻译单元)   qcanpool ← 整库删除（K5 到期）
  ├─ ribbon* (deprecated)         └─ ribbon*      ← 物理移除（A3）
  └─ fancy*/tiny*/mini* (未标注)   ├─ 7 个通用件   ← 迁入 qxapp（A1，留同名转发头）
                                  └─ 其余 11 个   ← 直接删除（A2）
qxcore  (Settings/Logger)       qxcore                            qxcore (+ i18n)
qxtheme                         qxtheme                           qxtheme
qxapp   (AppShell 骨架)          qxapp (+ 7 个通用件 ← A1)          qxapp (+ Toast / 属性编辑器 / 设置对话框)
qxribbon                        qxribbon (唯一 ribbon)             qxribbon
qxdock                          qxdock                            qxdock
qxwindow                        qxwindow (+ 单测)                  qxwindow
src/modules  (空)               src/modules (空)                  src/modules (空)          ← M7
src/plugins  (骨架)             src/plugins (骨架)                src/plugins (骨架)        ← M7
projects/template (qmake)       + CMake 模板 + scripts/new-project （不变）
examples/qcanpool (4 个示例)     同上                              ← 整目录删除
```

> **3.2 的收尾** = `qcanpool` 库（含 7 个转发头）整库消失，`src/libs` 收敛到 **6 个库**；
> 同时 `qxcore` 长出 i18n、`qxapp` 长出 Toast / 属性编辑器 / 设置对话框。
> 工作区预设与状态栏进度属 3.3。
