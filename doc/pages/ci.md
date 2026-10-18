# CI 与在线站点

本页说明两条 GitHub Actions 工作流的结构、平台坑，以及本机如何查看 CI 结果。提交与格式门禁的
本地跑法见 [构建与安装](build.md)。

## 工作流总览

| 文件 | 触发 | 内容 |
| :--- | :--- | :--- |
| `.github/workflows/ci.yml` | push / PR 到 `master` / `release-*` | 矩阵构建 + 测试、clang-format 门禁、SDK 消费门禁、覆盖率 / 消毒器门禁、clang-tidy（报告） |
| `.github/workflows/pages.yml` | `doc/**` `src/libs/**` `demos/**` `cmake/**` 根 `CMakeLists.txt` 自身 | Doxygen 站点 + wasm demo，合并后发 GitHub Pages |

## ci.yml 矩阵

- 平台 × Qt：`ubuntu` / `windows` / `macos` 各一条 **Qt 6.8.1**，外加 `ubuntu` × **Qt 5.15.2** 与
  `ubuntu` × **Qt 6.12.0**。
- `macOS` 那条腿**只构建 `qxwindow` 与 `qxdock`** 且不跑测试（见
  [已知限制](limitations.md#macos-builds-qxwindow-and-qxdock-only)）；判据是「文件里有没有
  `#ifdef Q_OS_MAC`」，不是「谁实现了平台后端」。
- `pages.yml` 另有一条 wasm 腿（钉 Qt 6.8.3，不门禁）。

## clang-format 门禁（`format` 作业）

- 固定 `clang-format==21.1.8`（`pip install 'clang-format==21.1.8'`）；版本**不要升级**——
  CI 与本地必须一致，否则 `dry-run` 结果对不上。
- 覆盖 `git ls-files` 的 `src` / `demos` / `examples` / `tests` / `projects` 五组 `*.h` `*.cpp`。
  **必须用 `git ls-files` 取清单**——`find` 会把被忽略的构建产物也算进来。
- 该作业同时收容**纯文本静态检查**：`scripts/check-template-sync.py`（比对 `projects/template` 与
  `scripts/new-project`，1 秒级，漂移应在落地一分钟内被点名）。
- ⚠️ **作业 id `format` / 显示名 `clang-format` 别改**：分支保护可能把它列为 required check，
  改名会把 `master` 直接堵死。**新门禁加步骤，不要为此改名。**
- ⚠️ **glob 有空洞**：`'examples/**/*.cpp'` 命中不到 `examples/main.cpp`——git 的 `**` 至少要吃掉一层
  目录名。已补 `'examples/*.h' 'examples/*.cpp'`，**别当冗余删掉**。往任何被门禁覆盖的树里加
  「根下文件」都要复查 glob。

本地跑法（固定版本 + `git ls-files`）：

```bash
FILES=$(git ls-files 'src/**/*.h' 'src/**/*.cpp' 'demos/**/*.h' 'demos/**/*.cpp' \
  'examples/**/*.h' 'examples/**/*.cpp' 'tests/**/*.h' 'tests/**/*.cpp' \
  'projects/**/*.h' 'projects/**/*.cpp')
clang-format --style=file --dry-run --Werror $FILES
```

## build 作业（含两条 SDK 门禁）

`ubuntu / Qt 6.8.1` 上额外跑两条「安装 → 消费」往返，作为可消费性门禁：

- `projects/consume`：装好 SDK 后 `find_package(QtCanpool)` → 编译 → 链接 → 运行。
- `scripts/new-project`：真实生成一个工程并编译（`test -x` 断言产物）。

## native 作业（qxwindow native，报告态、非门禁）

- 跑 `QT_QPA_PLATFORM=windows` 下的 `tst_qxwindow_native`，独立 `windows-latest`、
  `-DWITH_DEMOS=OFF`、只构建一个 target。
- **不走 ctest**：全 SKIP 的套件退出码也是 0，而 ctest 不把通过用例输出写进日志 ⇒
  「平台插件没加载、全跳了」与「窗口过程正常」是同一个绿勾。改为直接跑 exe，用 `::error::` 判两种
  假通过：**SKIP 计数 > 0**、**报告文件为空 / 不存在**。
- 报告用 `-o <file>,txt`（Windows 下该平台插件 stdout 为空）；包 `timeout 120`（这类测试**挂**
  而不是**失败**）。
- ⚠️ 该 `run:` 块由 `bash -e -o pipefail` 执行；`grep -c` 零匹配时退出 1 会静默中止健康运行。
  桩跑必须复刻 runner 的 shell 选项，并断言四种情形的退出码（全过 0 / 全跳 1 / 用例失败 1 / 无报告 1）。

## quality 作业（门禁）

单独 `ubuntu-latest / Qt 6.8.1`，只管门禁，避免拖慢整条矩阵：

- **覆盖率**：Debug + `WITH_COVERAGE=ON`、ctest、`lcov` 采集过滤，报告上传 artifact，**不设阈值**
  （决策 K8；本地怎么跑见 [构建与安装](build.md#覆盖率与动态分析)）。
- **消毒器**：Debug + `WITH_SANITIZE=ON -DSANITIZE_FLAGS=address,undefined`，跑完整 ctest；
  `ASAN_OPTIONS=detect_leaks=0`（Qt 退出时的保留分配是噪声），
  `UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1`。
- ⚠️ 产物上传步骤都带 `if: always()`：否则前一步红了，日志没生成，artifact 报 "No files were found"。

## tidy 作业（clang-tidy，报告态、非门禁）

- 只 configure（`-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`、`WITH_DEMOS/TESTS=OFF`）**不编译**——
  clang-tidy 只要 `compile_commands.json`。
- `-checks='-*,bugprone-*,performance-*'`（clang-analyzer 太慢暂不开），`continue-on-error: true`。
- `xargs -P $(nproc)` 并行，**一文件一份日志再 cat**（clang-tidy 一条诊断分多次 `write()`，
  共用一条流会交错成乱码）。
- 定向分析：PR 用 `git merge-base origin/$BASE_REF HEAD`；**push 用 `github.event.before`** →
  `git diff --name-only --diff-filter=d -- src/libs`。未改动 `src/libs` 的推送直接跳过；改动含头文件
  则退回全树（头会波及每个 TU，定向分析会少报）。
- 摘要按 check 分组 + 前 20 条写进 `$GITHUB_STEP_SUMMARY`；同时打印墙钟秒数。
- ⚠️ **为什么拆出来**：它原先排在 `quality` 之后，一个不阻塞的步骤占着关键路径。教训——
  `continue-on-error` 的东西不要和门禁共用作业。改造后 `quality` 13.2 → 3.4 min（−74%），
  整条流水线 13.2 → 5.2 min（−60%）。

## pages.yml（文档 + wasm + 部署）

- `docs` 作业装 Doxygen + Graphviz 建站点（PR 只建不发）；`wasm` 作业装 Qt wasm 套件 + emsdk 3.1.56
  建 `AppShellDemo`；`deploy` 把两者合并发 Pages。
- 🔴 **切换主线时别忘了两处 GitHub 配置**（主线由 `release-3.x` → `master` 时踩到）：
  ① `pages.yml` 的 `deploy` 条件已放开到 `master || release-*`；
  ② **仓库的 `github-pages` 环境另有「部署分支白名单」**（workflow 文件改不了）——只放行 `release-3.x`
  时 `deploy` 作业会 **0 步骤、1 秒失败、无日志**，看着像 workflow 写错。修法是把 `master` 加进该环境的
  部署分支策略。
- 已启用：文档占根 <https://canpool.github.io/qtcanpool/>，demo 占 `/demo/`。

## 本地构建速度

`qt5` / `qt6` CMake preset 已切 **Ninja**（2026-10-18）：空跑从 MinGW Makefiles 的近两分钟纯依赖
扫描降到毫秒级，且按核数自动并行——「本地构建一律带 `--parallel`」的旧规则随之作废。全量构建
约定 `--parallel 16`（作者本机 32 核 / 31.7GB，避开 g++ 内存墙，理由见
[构建与安装](build.md)）；手动 MinGW Makefiles 才必须带 `--parallel`（不带即串行）。

## 平台坑

CMake 平台分支逐条对照 qmake 的 `.pri`：

1. **AGL**——Qt < 6.9 的 `FindWrapOpenGL.cmake` 硬编码 `-framework AGL`，macOS 26 已移除 →
   `cmake/FindWrapOpenGL.cmake` 覆盖 + 顶层 `list(PREPEND CMAKE_MODULE_PATH ...)`。
2. 测试 exe 必须与 DLL 同目录（Windows 无 RPATH）。
3. **qxdock X11** 需 `Qt5::GuiPrivate` + `xcb` + `linux/dockfloatingtitlebar.cpp`。
4. macOS 用 `QX_WINDOW_NATIVE` 且 `windowcontext_qt.cpp` 只在 `UNIX AND NOT APPLE` 编译。

## 如何查看 CI 结果（本机无 `gh`）

- 未认证即可读 check-runs（含 conclusion）：
  `curl -s https://api.github.com/repos/canpool/qtcanpool/commits/<sha>/check-runs`。
- **作业日志需要 token**（取 GCM token 后 `curl -sL -H "Authorization: token $TOKEN" .../actions/jobs/<job_id>/logs`）。
  本机无 `gh`、且 github 需要 unset 代理；取 token / 重试的具体操作属本机环境细节，记录在记忆中、不进仓库。
- 三条通路的匿名可读性：

  | 通路 | 匿名 | 说明 |
  | :--- | :--- | :--- |
  | check-runs 元数据 | ✅ | `commits/<sha>/check-runs` |
  | **注解 annotations** | ✅ | `check-runs/<id>/annotations` —— **唯一能匿名读到「作业说了什么」的通路** |
  | `GITHUB_STEP_SUMMARY` | ❌ | 即 check-run 的 `output.summary`，匿名取回全为空 |
  | 作业日志 | ❌ | 匿名 403，要 token |

  ⇒ **想让本机脚本读到某个作业的产出，就让它 `echo "::notice::…"`**（GHA 会把它变成注解）。
  多行用 `%0A`、字面 `%` 写成 `%25`。别用 `paste -d'%0A'` 拼（取的是字符列表并轮换，会用 `%`/`0`/`A` 依次分隔）。

## 消毒器的本地局限

- 🔴 **本机 MinGW GCC 无 `libubsan`/`libasan`** → 消毒器**无法本地复现**，只能靠 CI 的 `quality` 腿。
  **本地 ctest 全绿 ≠ 没有 UB。**
- 🔴 `UBSAN_OPTIONS=halt_on_error=1` **必须保留**：UBSan 默认 recover（打印后继续、进程正常退出），
  去掉它 ctest 会误绿。代价是一次 CI 只报第一条 → 「修一处、露一处」，每轮约 9 分钟。
