# 管理规范（Contributing）

> 本文件是 QtCanpool **过程规范的单一权威来源**。其它页面只讲技术细节：
> [架构与约定](architecture.md) 讲库分层与命名、[构建与安装](build.md) 讲编译、[发布流程](release.md) 讲发版动作、
> [已知限制](limitations.md) 讲刻意不改的行为、[迁移指南](migration.md) 讲跨版本迁移。
> 凡过程约定，以此文件为准；本文件与任何记忆/笔记冲突时，以本文件（仓库内）为准。

## 1. 提交规范

- **格式**：Conventional Commits —— `type(scope): subject`。**提交信息与代码注释一律英文**（`.md` 不限）。
- **type**：`feat` / `fix` / `docs` / `style` / `refactor` / `perf` / `test` / `build` / `ci` / `chore` / `revert`。
- **scope**：`qxcore` / `qxtheme` / `qxcontrols` / `qxfluent` / `qxmaterial` / `qxwindow` / `qxribbon` / `qxdock` / `qxplugin` / `qxapp` / `themes` / `icons` / `modules` / `project` / `ci` / `test` / `docs`。其中 `themes` 专指 `tools/themes/` 与 `res/themes/` 这套预置主题（生成器 + 生成物），不属于任何库；`icons` 专指图标规范与它的门禁（`scripts/icons-policy.txt`、`scripts/check-icons.py`、`doc/pages/icons.md`），同样不属于任何库。
- **原子提交**：一笔一件事。
- **每笔提交前必做**：按 §8 的判据选取验证范围（第 0 级不构建，其余最少一腿构建），
  clang-format 过改动文件，并在提交信息或回复里写明跑了哪一级。
- **两远程**：`origin` = gitee、`github`；一笔推两远程。

## 2. 文档规范（v1）

> 一句话：**代码＋库 CHANGELOG＋台账同笔（台账随代码走，不另开笔）｜ 叙事文档一组收口一笔 `docs(<scope>)` ｜ 版本号在本版第一笔代码之前单独一笔（开工） ｜ 根 CHANGELOG 发版时与三本账同笔 ｜ 其余各自成笔。**

| 产物 | 落点 | 成笔时机 / 粒度 | 与谁同笔 |
| :--- | :--- | :--- | :--- |
| 代码（`.h/.cpp/.qrc`/测试/示例） | `src/**` `tests/**` `examples/**` `demos/**` | 每个可验证的最小原子改动一笔 | 与**同改动的库 CHANGELOG ＋ 台账** 同笔 |
| 库级 CHANGELOG | `src/libs/<lib>/CHANGELOG` | 每次改动只要「需登记」就写（顶端 `Unreleased` 段，`Feature`/`Interface`/`Optimize`/`Bugfix` 体例） | 随代码同一笔（不另开笔、不按组） |
| 台账 | `doc/design/<minor>/<ver>-TASKS.md`（如 `4.7/4.7.0-TASKS.md`） | **随本次改动即时更新**（落了哪条写哪条，不按组、不攒批） | 随代码同一笔（漏写只可补一笔、与代码同笔补） |
| 叙事文档（限制页、迁移、路线叙事、README） | `doc/pages/*.md`、`doc/design/*.md` 叙事段、README | 一组收口时统一提交，不逐条提 | 组末单独一笔 `docs(<scope>)`；不与台账同笔 |
| 根 CHANGELOG | `CHANGELOG` | 发版时一次性写 | 与项目版本号前进同笔 |
| 项目版本 | `cmake/QtCanpoolBranding.cmake`（三字段）+ `ports/qtcanpool/vcpkg.json` + `conanfile.py` | **本版第一笔代码之前**前进，三处齐升、值相等（`COMPAT_VERSION` = 项目版本）；五步开工清单见 [发布流程](release.md) §零 | 单独一笔 `chore(project): open <ver>`，**不与任何代码同笔** |

**一个工作段落的顺序**

```
实现 → 构建 + ctest + clang-format → 写库 CHANGELOG ＋ 台账
→ 【代码 ＋ 库 CHANGELOG ＋ 台账】一笔 → 推两远程
→ 一组收口、若有叙事文档要写 → 【叙事文档】一笔 docs(<scope>) → 推两远程
```

**`docs(<scope>)` 的 scope 怎么写（内容驱动，绝不写死）**

- 只动了限制页 → `docs(limitations)`；只动了迁移/路线叙事 → `docs(migration)` / `docs(roadmap)`。
- 混合（限制页 + 路线叙事）→ 取实质主因的 scope，或两个都写（如 `docs(limitations,migration)`）；仍是一笔，不按文件拆。
- ⚠️ 防止写死成 `docs(plan)`。任何 `docs(...)` 都行，关键词跟着内容走。

**例外与免责**

- **已推送的不追溯**：事后发现库 CHANGELOG / 台账漏写，只能补一笔（与代码同笔补，scope 用 `docs(changelog)` / `docs(design)`），不改写历史。
- **台账纪律**：在研版本任务动态规划，中途不存在「收口」一说；按「当前已落地条目」写，新工作照常追加，版本未结算前开放。归因标签（`stretch` / 可延后）一律标「助手建议」，不得写成「原定 / 既定 / 原留」，不得命名具体后续版本号。

### 2.1 台账的目录与两层结构（**4.7 起**，作者 2026-10-22 定）

K36 定了 patch 是常态（一个 minor 系列会有很多个 patch），扁平的 `doc/design/<minor>-TASKS.md`
因此必然越写越长——4.4 一个 minor 三个 patch 就撑到 1717 行，涨的全是执行流水，把定位与
决策淹在里面。所以台账改成**一个 minor 一个目录、一个 patch 一份文件、分两层**：

```
doc/design/4.7/
  4.7-TASKS.md        # 系列层：4.7.x 期间唯一持续增长的，只涨指针与结论
  4.7.0-TASKS.md      # 单 patch：分解 + 执行记录 + 结算，发版封版
  4.7.1-TASKS.md      # 下一个 patch 开新文件，不回写上一个
```

- **系列层** `<minor>/<minor>-TASKS.md`：该 minor 的定位与硬约束、K 决策、**候选池**（下一个
  patch 从这取材）、跨 patch 进度表。
- **patch 层** `<minor>/<minor>.<patch>-TASKS.md`：本 patch 的范围、任务分解、执行记录、结算。
- ⚠️ **patch 台账发版即封版**：发版那一笔之后，新工作写进下一个 patch 的文件，**不回写上
  一个**——只建目录不封版，只是把「一个文件涨到 2500 行」换成「五个文件各涨到 500 行」。
- **每个 patch 都要有立项**，不是装东西的桶：范围（为什么是这个 patch）+ **兼容面核对一行**
  ——K36 说 patch 不能改签名 / 删 API / 改名（**默认行为可以改**：按 patch 走，且在库
  `CHANGELOG` 里单独成条），开工时核对，越界要么改判 minor、要么把该条挪进下一个 minor 的候选池。
  ⚠️ **内部重构不在这三项里**（拆大文件、抽纯逻辑类、调私有类可见性）：公开面不变
  ⇒ 它就是 patch +1 的常态，**不要攒到下一个 minor**（2026-10-22 作者裁定，见 ROADMAP K36 表）。
- **历史不追溯**：4.6 及更早保持扁平（4.4.1 / 4.4.2 的内容长在 `4.4-TASKS.md` 里，拆它等于
  重写历史）。4.7 是唯一的例外——它当时未发版，故按新制归档。
- 命名保留 `-TASKS.md` 后缀（`**/*-TASKS.md` 仍可 glob），patch 文件写全三段：单独打开或被
  grep 到时不依赖目录名才知道自己是谁。
- `doc/design/` 不进 Doxygen（`INPUT` 只有 `src/libs` 与 `doc/pages`），改目录结构不影响文档站。

## 3. 注释与文档的写法

- **不写活数据的基数**（主题数 / 文件数 / 条目数 / 库数……）。数字是派生事实：改了数据不会有人回来改注释——实例是新增 accent 家族时 `qxthemebundle.cpp` 的注释更新了、同名头文件的「four families / eight themes」没有，`architecture.md` 的「当前 6 个库」也已过期。写法改为**指向唯一事实源**（qrc、生成器的 `FAMILIES`、紧挨数据的枚举）或**写结构**（「每族明暗各一套」）。
- **例外**：历史留档（库 CHANGELOG 条目、migration、限制页里的测量记录、台账）记的就是某个时点的事实，可以带数字。

## 4. 库 CHANGELOG 活文档（K24）

- 改动**落地时就写**进该库 `CHANGELOG` 顶端 `Unreleased` 分节（`Feature`/`Interface`/`Optimize`/`Bugfix` 体例），含**不动号的行为修复**。
- **修缺陷一律写 `Bugfix`**，不写 `Fix` / `Fixed`。两者曾在不同库里各写一种，同一个文件里上下两个版本块互不一致；2026-10-23 全部归一（含历史段的小节名，只改名字、不动条目内容）。
- 该库**落号时**把 `Unreleased` 改名成新版本号，与改号同一笔提交。
- **不追溯**：已发布的段一字不动。
- `qxcore` / `qxtheme` / `qxapp` / `qxplugin` **首次有改动时才建**文件。
- 一个版本块里每种小节只留一个：追加并进已有同名小节、别另起同名小节；合并只搬条目不重写，小节顺序按首次出现。

## 5. 编码与行尾

- 全仓行尾统一 **LF**；Python 写文件一律二进制模式（否则 `\n` → CRLF）。
- 源文件一律**不带 BOM 的 UTF-8**；MSVC 编码问题由根 `CMakeLists.txt` 对 `MSVC_VERSION >= 1910` 加 `/utf-8` 解决（只加 `/source-charset:utf-8` 是错的：窄字面量会变系统代码页）。改写时别顺手保留 BOM；散文 em dash / 弯引号换 ASCII。

## 6. SPDX

- 新文件 / 补头写在文件最顶部：
  `/** … Copyright (C) <年> maminjie <canpool@163.com> … SPDX-License-Identifier: MulanPSL-2.0 … **/`。
- 两类存量不改：`qxwindow` 整库 `Apache-2.0`、`qtproject{,_gui}_pch.h` 的 `GPL-3.0`。

## 7. clang-format 门禁

- 版本 **21.1.8**，`--style=file`（根 `.clang-format`）。CI 强制门禁；提交前对改动文件跑一遍，**勿升级**。

## 8. 构建与验证

- 构建走 CMake preset（`qt6` / `qt5`，Ninja 生成器，自动并行）；**全量构建带 `--parallel 16`**——作者本机 32 核按核数满载会撞 g++ 内存墙（症状是测试无输出失败一类 flaky）。手动 MinGW Makefiles 才必须带 `--parallel`：不带即单核串行。
- **验证范围按改动分级，不一律两腿**。判据一句话：**这处改动有没有可能只在某一个 Qt 版本、
  或某一个平台上编不过、或行为不同？答不出「不可能」就跑两腿。**

| 级别 | 改动 | 验证 |
| :--- | :--- | :--- |
| 0 | 只动 `doc/`、`.md`、CHANGELOG 文案 | 不构建 |
| 1 | 源文件里**只改注释**；或改不涉及 Qt API 与平台分支的内部实现 | 一腿增量构建 + `scripts/check-encoding.py`；动了行为才跑相关 ctest 子集 |
| 2 | 新增 / 替换 Qt API 或容器用法、改 `#if QT_VERSION` / `Q_OS_*`、公开头里的 inline / 签名 / 枚举 | 两腿 100% 构建 + 两腿全量 ctest（`QT_QPA_PLATFORM=offscreen`） |
| 3 | CMake / `.qrc` / `header.list` / 资源 / 既有测试断言 | 两腿 100% 构建 + 两腿全量 ctest |

- **第 1 级凭什么能省**：注释不进编译器，两腿的编译产物逐字节相同；注释唯一能踩的是
  「写了坏字节」（`scripts/check-encoding.py` 全树扫 UTF-8 与 BOM，实测 765 个文件 1.7 秒）
  与格式（clang-format）。但**「只改注释」必须是确定的**，不是「看着像注释」——拿不准就按第 2 级。
- **两腿的价值在提交前，不是唯一观测通道**：CI 的 Qt 5.15 腿跑在 `ubuntu-latest`
  （`Qt5 + MSVC` 在 runner 的 toolset 上编不过，故没有 Windows 腿），所以「Windows + Qt 5」
  只有本机能验证；而 Qt 版本差异里与平台无关的那部分（如 Qt 6 独有的 `QList<QString>(int)`）
  CI 也能抓——本机两腿是为了**不让红提交先落进主线**（教训：B1 用 Qt6 独有 API，Qt5 侧连编不过）。
- **判据要留痕**：提交信息或回复里写明级别，如 `Verified: qt6 only (comment-only)`。
  「只跑一腿」是可审计的声明，不是沉默省掉的步骤；说不清为什么够，就按上一级跑。
- 覆盖基线含 demos 截图判定；视觉回归（demo 截图）为 opt-in，不影响门禁。

## 9. 分支、版本与进号

- `master` 是主线（K18）；不建 `release-4.x`。
- **版本序（K34）**：4.3 `qxwindow` → 4.4 不破坏 API 的内部重构 → 4.5 功能升级（可破坏 API）→ … → 5.0 移除 Qt 5。4.x 长生命周期主版本（类 Qt 5.0→5.15）。4.5 不定位 LTS（LTS 留到 4.6+ 之后才定）。
- **进号判据（附录 B / K11）**：看「这个库本期有没有修复 / 增量」，不是看「头文件动没动」。构建接线（删版本守卫 / 删 `header.list`）不进号；纯 `.cpp` 修复照样进 patch。结算时逐库问「本期有没有修复」。
- 版本号在「新版本开发刚开始」时前进，**在本版第一笔代码之前**：`cmake/QtCanpoolBranding.cmake`（三字段）
  + `ports/qtcanpool/vcpkg.json` + `conanfile.py` **三处齐升、值相等**（`COMPAT_VERSION` = 项目版本）。
  仓库**没有** `qtproject.pri` 之类的第 4 处。完整五步（定本版号 / 三处齐升 / 台账开版 / 库号不动 /
  两腿验证）与核对命令见 [发布流程](release.md) §零；库号在开工时一律不动。

## 10. tag 与发布

- tag **无 `v` 前缀**、附注 `release <ver>`、显式钉 SHA。
- GitHub Release 标题 `QtCanpool <ver>`、正文**英文**（不贴中文 `CHANGELOG` 原文；要么改写为英文，要么另写英文摘要）。
- 两远程推送；完整步骤见 [发布流程](release.md)。

## 11. 命名与敏感信息

- 公共 API / 枚举 / 主题 id 按「**形态或几何**」命名、不用产品名（主题族 `classic` / `soft` / `flat` / `slate` / `accent`；`RibbonStyle` 四值 `TabBar…Style`）。改名先查新词是否被占用。
- **敏感信息**：某商用 Ribbon 组件名属敏感信息，提交 / 注释 / 文档 / 台账一律不得出现（连厂商类名也不列）；历史存量不动，新增必须干净。

## 12. 架构纠偏判据（K 组）

- 四条值得推翻的设计判据：**越界**（库把本属应用层的职责打包进自己，使应用无从替换）/ **双轨**（同一件事在库里有两套互不相干实现且彼此不通气）/ **隐式压制**（库的行为在没有对外 API 表达的前提下压过应用的设置）/ **开关错配**（非外观的开关改变外观或行为）。
- **「未导出 ⇒ 非 API」**：有无 `QX_<LIB>_EXPORT` 才是判据；磁盘上有 `_p.h` ≠ 可用。
- 详细台账与条目见 `doc/design/<minor>/<ver>-TASKS.md` 的 K 组（如 `4.5-TASKS.md`；4.7 起在 `4.7/` 目录里）。

## 13. CI 概览

- GitHub Actions：门禁作业（`clang-format` / `source encoding` / `clang-tidy` / `quality` / 各平台 `build` + `ctest`）与非门禁步骤分离，互不共用作业。
- 改 workflow 后本机 PyYAML 解析 + 桩跑该步骤（PyYAML 只在系统 Python 3.12）。
- 视觉回归（demo 截图）为 opt-in，不加环境变量即跳过，不拦门禁。

## 引用

- 技术细节：[架构与约定](architecture.md) · [构建与安装](build.md) · [发布流程](release.md) · [已知限制](limitations.md) · [迁移指南](migration.md) · [主题引擎](theming.md) · [图标规范](icons.md)
- 任务台账：`doc/design/<minor>/<ver>-TASKS.md`（如 `4.5-TASKS.md`、`4.7/4.7.0-TASKS.md`），目录与两层结构见上文 §2.1；路线：`doc/ROADMAP-3.x.md`
