# 管理规范（Contributing）

> 本文件是 QtCanpool **过程规范的单一权威来源**。其它页面只讲技术细节：
> [架构与约定](architecture.md) 讲库分层与命名、[构建与安装](build.md) 讲编译、[发布流程](release.md) 讲发版动作、
> [已知限制](limitations.md) 讲刻意不改的行为、[迁移指南](migration.md) 讲跨版本迁移。
> 凡过程约定，以此文件为准；本文件与任何记忆/笔记冲突时，以本文件（仓库内）为准。

## 1. 提交规范

- **格式**：Conventional Commits —— `type(scope): subject`。**提交信息与代码注释一律英文**（`.md` 不限）。
- **type**：`feat` / `fix` / `docs` / `style` / `refactor` / `perf` / `test` / `build` / `ci` / `chore` / `revert`。
- **scope**：`qxcore` / `qxtheme` / `qwindow` / `qxribbon` / `qxdock` / `qxplugin` / `qxapp` / `modules` / `project` / `ci` / `test` / `docs`。
- **原子提交**：一笔一件事。
- **每笔提交前必做**：重新构建 + ctest 全绿 + clang-format 过改动文件。
- **两远程**：`origin` = gitee、`github`；一笔推两远程。

## 2. 文档规范（v1）

> 一句话：**代码＋库 CHANGELOG＋台账同笔（台账随代码走，不另开笔）｜ 叙事文档一组收口一笔 `docs(<scope>)` ｜ 根 CHANGELOG 与版本号发版同笔 ｜ 其余各自成笔。**

| 产物 | 落点 | 成笔时机 / 粒度 | 与谁同笔 |
| :--- | :--- | :--- | :--- |
| 代码（`.h/.cpp/.qrc`/测试/示例） | `src/**` `tests/**` `examples/**` `demos/**` | 每个可验证的最小原子改动一笔 | 与**同改动的库 CHANGELOG ＋ 台账** 同笔 |
| 库级 CHANGELOG | `src/libs/<lib>/CHANGELOG` | 每次改动只要「需登记」就写（顶端 `Unreleased` 段，`Feature`/`Interface`/`Optimize`/`Bugfix` 体例） | 随代码同一笔（不另开笔、不按组） |
| 台账 | `doc/design/<ver>-TASKS.md` | **随本次改动即时更新**（落了哪条写哪条，不按组、不攒批） | 随代码同一笔（漏写只可补一笔、与代码同笔补） |
| 叙事文档（限制页、迁移、路线叙事、README） | `doc/pages/*.md`、`doc/design/*.md` 叙事段、README | 一组收口时统一提交，不逐条提 | 组末单独一笔 `docs(<scope>)`；不与台账同笔 |
| 根 CHANGELOG | `CHANGELOG` | 发版时一次性写 | 与项目版本号前进同笔 |
| 版本号 + 打包清单 | `QtCanpoolBranding.cmake` + `qtproject.pri` + `vcpkg.json` + `conanfile.py` | 新版本开发刚开始时前进，三处齐升、值相等 | 同笔（`chore(release)`） |

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

## 3. 库 CHANGELOG 活文档（K24）

- 改动**落地时就写**进该库 `CHANGELOG` 顶端 `Unreleased` 分节（`Feature`/`Interface`/`Optimize`/`Bugfix` 体例），含**不动号的行为修复**。
- 该库**落号时**把 `Unreleased` 改名成新版本号，与改号同一笔提交。
- **不追溯**：已发布的段一字不动。
- `qxcore` / `qxtheme` / `qxapp` / `qxplugin` **首次有改动时才建**文件。
- 一个版本块里每种小节只留一个：追加并进已有同名小节、别另起同名小节；合并只搬条目不重写，小节顺序按首次出现。

## 4. 编码与行尾

- 全仓行尾统一 **LF**；Python 写文件一律二进制模式（否则 `\n` → CRLF）。
- 源文件一律**不带 BOM 的 UTF-8**；MSVC 编码问题由根 `CMakeLists.txt` 对 `MSVC_VERSION >= 1910` 加 `/utf-8` 解决（只加 `/source-charset:utf-8` 是错的：窄字面量会变系统代码页）。改写时别顺手保留 BOM；散文 em dash / 弯引号换 ASCII。

## 5. SPDX

- 新文件 / 补头写在文件最顶部：
  `/** … Copyright (C) <年> maminjie <canpool@163.com> … SPDX-License-Identifier: MulanPSL-2.0 … **/`。
- 两类存量不改：`qxwindow` 整库 `Apache-2.0`、`qtproject{,_gui}_pch.h` 的 `GPL-3.0`。

## 6. clang-format 门禁

- 版本 **21.1.8**，`--style=file`（根 `.clang-format`）。CI 强制门禁；提交前对改动文件跑一遍，**勿升级**。

## 7. 构建与验证

- 构建一律带 `--parallel`（MinGW Makefiles，不带即串行）。
- **两腿都要跑**：Qt 5.15 与 Qt 6 各 100% 构建 + ctest（`QT_QPA_PLATFORM=offscreen`），别只跑 qt6（教训：Qt 6 独有 API 在 Qt 5 侧连编不过）。新增 / 改写代码先问「有没有 Qt 6 独有 API？」。
- 覆盖基线含 demos 截图判定；视觉回归（demo 截图）为 opt-in，不影响门禁。

## 8. 分支、版本与进号

- `master` 是主线（K18）；不建 `release-4.x`。
- **版本序（K34）**：4.3 `qxwindow` → 4.4 不破坏 API 的内部重构 → 4.5 功能升级（可破坏 API）→ … → 5.0 移除 Qt 5。4.x 长生命周期主版本（类 Qt 5.0→5.15）。4.5 不定位 LTS（LTS 留到 4.6+ 之后才定）。
- **进号判据（附录 B / K11）**：看「这个库本期有没有修复 / 增量」，不是看「头文件动没动」。构建接线（删版本守卫 / 删 `header.list`）不进号；纯 `.cpp` 修复照样进 patch。结算时逐库问「本期有没有修复」。
- 版本号在「新版本开发刚开始」时前进：`QtCanpoolBranding.cmake` + `qtproject.pri` + `vcpkg.json` + `conanfile.py` 三处齐升、值相等（`COMPAT_VERSION` = 项目版本）。

## 9. tag 与发布

- tag **无 `v` 前缀**、附注 `release <ver>`、显式钉 SHA。
- GitHub Release 标题 `QtCanpool <ver>`、正文**英文**（不贴中文 `CHANGELOG` 原文；要么改写为英文，要么另写英文摘要）。
- 两远程推送；完整步骤见 [发布流程](release.md)。

## 10. 命名与敏感信息

- 公共 API / 枚举 / 主题 id 按「**形态或几何**」命名、不用产品名（主题族 `classic` / `soft` / `flat` / `slate`；`RibbonStyle` 四值 `TabBar…Style`）。改名先查新词是否被占用。
- **敏感信息**：某商用 Ribbon 组件名属敏感信息，提交 / 注释 / 文档 / 台账一律不得出现（连厂商类名也不列）；历史存量不动，新增必须干净。

## 11. 架构纠偏判据（K 组）

- 四条值得推翻的设计判据：**越界**（库把本属应用层的职责打包进自己，使应用无从替换）/ **双轨**（同一件事在库里有两套互不相干实现且彼此不通气）/ **隐式压制**（库的行为在没有对外 API 表达的前提下压过应用的设置）/ **开关错配**（非外观的开关改变外观或行为）。
- **「未导出 ⇒ 非 API」**：有无 `QX_<LIB>_EXPORT` 才是判据；磁盘上有 `_p.h` ≠ 可用。
- 详细台账与条目见 `doc/design/<ver>-TASKS.md` 的 K 组（如 `4.5-TASKS.md`）。

## 12. CI 概览

- GitHub Actions：门禁作业（`clang-format` / `source encoding` / `clang-tidy` / `quality` / 各平台 `build` + `ctest`）与非门禁步骤分离，互不共用作业。
- 改 workflow 后本机 PyYAML 解析 + 桩跑该步骤（PyYAML 只在系统 Python 3.12）。
- 视觉回归（demo 截图）为 opt-in，不加环境变量即跳过，不拦门禁。

## 引用

- 技术细节：[架构与约定](architecture.md) · [构建与安装](build.md) · [发布流程](release.md) · [已知限制](limitations.md) · [迁移指南](migration.md) · [主题引擎](theming.md)
- 任务台账：`doc/design/<ver>-TASKS.md`（如 `4.5-TASKS.md`）；路线：`doc/ROADMAP-3.x.md`
