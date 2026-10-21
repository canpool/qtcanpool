# 发布流程

这一页是**「发一次版本要做的全部动作」的唯一出处**。
[`doc/ROADMAP-3.x.md`](https://github.com/canpool/qtcanpool/blob/master/doc/ROADMAP-3.x.md) 的附录 B
与各 `doc/design/*-TASKS.md` 里的发布小节是**当时的执行记录**；规则若与本页不一致，以本页为准。

发布分四件事：**过闸门 → 定版本号 → 写三本账 → 推 tag 与 Release**。
前两件决定"能不能发"，后两件决定"发得对不对"。

---

## 一、发布前的两道闸门

### 1. CI 全绿

推送要发布的那个提交，等 `ci.yml`（`build` × 5、`format`、`quality`、`tidy`）与
`pages.yml`（`docs`、`wasm`、`deploy`）全部 `success`。

⚠️ **不要看「最近一次 CI 绿了」就发**——要看**那一个提交**的检查项。发布期间别的工作合并进来，
绿的是别人的提交。

### 2. tag 显式钉在验证过的那个提交上

```bash
git tag -a 4.2.0 -m "release 4.2.0" <验证过的 SHA>
```

**显式给出 SHA**，不要依赖"当前 HEAD"。历史上发布期间仍有提交落到 `master`（4.1 的 A1 就在
tag 之后落的），用 HEAD 会把 tag 指向一个**没跑过 CI**的提交。

---

## 二、先定版本号

每个库有**独立的演进号**（决策 K11）。规则三条：

0. **先定项目版本是 minor 还是 patch**（K36，完整表格见
   [`doc/ROADMAP-3.x.md` 附录 B](https://github.com/canpool/qtcanpool/blob/master/doc/ROADMAP-3.x.md)
   「兼容承诺与项目版本节奏」）：**默认 patch**；只有出现改签名 / 删 API / 改默认行为
   （且不是修缺陷）时才升 minor。⚠️ 本项目的 **patch 含新增能力**，与常见 semver 约定不同，
   发版公告里要写明。
1. **0.x 的语义**：`minor + 1` = 接口契约发生了变化性调整；`patch + 1` = 纯增量或修复。
   全部库都还在 0.x，**不承诺二进制兼容**——但**兼容性由项目版本承诺**：同一个项目 minor
   系列内，所有库的公开 API 只增不减不改签名（K36）。库号只回答「这个库本期动了多少」。
2. **每次项目发版结算一次**：本期**有实质改动**的库才前进号（修复、增量、内部重构都算，
   按附录 B 落在 `patch`：`patch + 1` = 纯增量或修复）；本期什么都没动的库**保持原号**。
   **只动构建接线、头文件清单、注释的不算。**

   ⚠️ **判据是「这个库本期有没有修复 / 增量」，不是「头文件动没动」** —— 这一点被记错过两次，
   故把先例写在这里。**4.4.0 的 `qxribbon` / `qxdock` / `qxwindow`：三个库全是纯 `.cpp` 修复、
   公开面一行没变，照样各进一个 patch**（见 `doc/design/4.4-TASKS.md` §七 D3：
   「真正驱动进号的是『库本期有代码改动』这一事实」「非『有头文件改动就进号』」）。
   **反方向**：附录 B 的现成例子是 3.2 删 `header.list` —— 三个库各记一笔、**不进号**；
   4.4.2 的 `qxtheme` / `qxwindow` 各只删掉一个「低于最低支持版本」的版本守卫，同样**不进号**。
   版本号本身就是「这个库本期有没有动」的声明——不是为了对齐而对齐。
3. **不追溯**：只在新发布时前进。已发布 tag 里是多少就是多少，不在事后回补。

**怎么判定"动了没有"**（唯一可靠的依据，别靠印象）：

```bash
git diff --name-only <上次tag>..HEAD -- 'src/libs/**/*.h'
```

⚠️ **这条命令给的是「候选名单」，不是结论**：`.h` 的**纯注释**改动——比如在声明处补一段说明——
也会被列出来，但它**不算** API 改动，要按**实质**判，不能只看文件名。
（4.2 的 A13 就是一例：`src/libs/qxdock/dockautohidecontainer.h` 只加了一段注释，
`qxdock` 因此不动号。）

⚠️ **按库逐个核时，不要写 `<lib>/**/*.h`**：`src/libs/qxdock/**/*.h` **什么都列不出来**。
git 的 `**` 至少要吃掉一层目录名，所以紧贴库目录、没有子目录的 `src/libs/qxdock/*.h`
**匹配不到**。（上面那条全树命令是安全的：那里的 `**` 吃掉的正是库名那一层。）
逐库核要用**目录 pathspec** 再过滤：

```bash
git diff --name-only <上次tag>..HEAD -- src/libs/qxdock/ | grep '\.h$'
```

4.2 的 A12 就是被这条咬到的：当时据 `qxdock/**/*.h` 为空判成"qxdock 没动 `.h`"，
实际它**有**一处 `.h` 改动（A13 那段注释）——结论侥幸相同（都不进号），**依据却是错的**。

**改号要改两处**：

| 位置 | 内容 |
| :--- | :--- |
| `<lib>/CMakeLists.txt` | `VERSION` |
| `<lib>/<lib>_global.h` | `QX_<LIB>_VERSION_{MAJOR,MINOR,PATCH}` 三个宏 |

⚠️ 这个号有**两个镜像**，历史上漏改过两次（`qxapp_global.h` 停在 0.0.1、`qxdock_global.h` 停在 0.1.0），
两处必须同时改。qmake 侧原有的两处 `.pri` 已随决策 K13 删除，不要再去找。

**项目版本另有三处**：`cmake/QtCanpoolBranding.cmake`、`ports/qtcanpool/vcpkg.json`、`conanfile.py`
（仓库已无 `qtproject.pri` 之类的第 4 处，不要去找）。三处必须**在「发布、打 tag」的那一刻一同前进、值保持相等**：

- `cmake/QtCanpoolBranding.cmake` 的三个字段 `VERSION` / `COMPAT_VERSION` / `DISPLAY_VERSION` 同时改、三者相等；
- `ports/qtcanpool/vcpkg.json` 的 `version`；
- `conanfile.py` 的 `version`。

文档站页脚取自 `QTPROJECT_DISPLAY_VERSION`（来自 branding 文件），不必单独改。

⚠️ **`COMPAT_VERSION` 始终等于项目版本**（本仓家规：每发一版三处齐升；git 历史从 3.0 到 4.3 每笔发布都如此）。它只声明「用本项目源码编出来的插件要求的最低 IDE 版本」，并不影响「4.4 的 IDE 照常加载 4.3 编的插件」——向后兼容由「不碰公开 `.h`」保证，不靠这个常量。

⚠️ **时机：版本号在「新版本开发刚开始」那一刻前进，与 Qt Creator 的惯例一致。** Qt Creator 在上一版发布后、下一版开发启动的首个提交就把 `QTCREATOR_VERSION` 提到下一版号；开发期源码就 carrying 下一版号，发布时只打 `git tag -a <ver> <SHA>`、版本号不再改。qtcanpool 作为 Qt Creator 分叉沿用同一规则：
**开始 4.4 开发时就把三处 `4.3.0 → 4.4.0`**，整个 4.4 开发期源码都是 `4.4.0`；发布 4.4.0 时只打 tag，版本号不动。
（注：本仓 4.3.0 实际是在发布提交里才升的——与上游不一致；自 4.4 起改为「开发刚开始就升」，与 Qt Creator 对齐。）

---

## 三、发版要写三本账

| 账本 | 视角 | 什么时候写 | 回答什么 |
| :--- | :--- | :--- | :--- |
| 根 `CHANGELOG` | 项目 | **发版时一次性写**（打 tag 那笔提交；版本号已在开发起点前进，不在这一笔） | 这一版用户要改什么 |
| 库级 `src/libs/<lib>/CHANGELOG` | 库 | **跟着改动走**（决策 K24） | 这个库动了什么 |
| [`doc/pages/versions.md`](versions.md) 的版本对照表 | 项目 × 库 | 发版时补一行 | 这一版各库是什么号 |

**库级 `CHANGELOG` 的三条记法**（K24）：

1. **落地时就写**：改动随提交写进该库 `CHANGELOG` 顶端的 `Unreleased` 分节，沿用该文件原有的
   `Feature` / `Interface` / `Optimize` / `Bugfix` 体例；
2. **落号时改名**：该库版本号前进时，把 `Unreleased` **改名成新版本号**，且与改版本号**同一笔提交**；
3. **不追溯**：已发布的段一个字不动。`qxcore` / `qxtheme` / `qxplugin` / `qxapp` 目前没有该文件，
   **首次有改动时才建**。

⚠️ 根 `CHANGELOG` 里**必须写明哪个库动了号、为什么**；**没有库动号也要写出来**——
留白会被读成"有人忘了"。

---

## 四、推送

### 三必需

`~/.gitconfig` 是个人全局配置，**不擅改**；用环境变量在单条命令里给：

```bash
export GIT_TERMINAL_PROMPT=0 GIT_CONFIG_NOSYSTEM=1 \
       GIT_CONFIG_COUNT=1 GIT_CONFIG_KEY_0=credential.helper GIT_CONFIG_VALUE_0=manager
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY
bash scripts/push.sh master
```

三个坑各自对应上面的一项：

| 症状 | 原因 | 对策 |
| :--- | :--- | :--- |
| push 无输出、挂死 | 沙箱拦住了凭据 GUI | 在**放开沙箱**的前提下执行 |
| `502 CONNECT tunnel failed` | 代理拦截 github | `unset http(s)_proxy` |
| 挂死、退出码 124 | 系统级 `helper-selector` 弹了 UI | `GIT_CONFIG_NOSYSTEM=1` |

`github.com:443` 会**间歇性**连不上（`Could not connect to server`），隔约 35–40 秒重试即可，
常需 5–8 次。⚠️ 重试循环里**不要用 `cmd | tail` 判断成功**——拿到的会是 `tail` 的退出码；
用 `cmd > log 2>&1; rc=$?`。

### tag 要单独推

**`scripts/push.sh` 只推分支、不管 tag。** tag 要手动推，并且**逐段核对**：

```bash
git push github 4.2.0
git push origin 4.2.0
git ls-remote --tags github 4.2.0
git ls-remote --tags origin 4.2.0
```

⚠️ **分支推得通 ≠ tag 推得通**：同一时刻 tag 可能连续失败（历史上出现过第 4 次才成功、也有一次就过的，
是运气不是修好）。两端都要核对，且核对的是 **tag 对象**的 SHA，不只是它指向的提交。

---

## 五、GitHub Release

- **标题统一 `QtCanpool <ver>`**——建 Release 时**显式传 `name`**，不要用默认标题（那只是裸版本号）；
- **正文一律英文**：不要把中文 `CHANGELOG` 原文贴上去，要么取条目**改写为英文**，要么另写一份英文摘要；
- 附注 tag 消息 `release <ver>` 保持英文。

---

## 六、本机没有 `gh` 时

CI 状态与 Release 都走 GitHub API。仓库里有两个**不入库**的辅助脚本（在 `build/` 下，被 `.gitignore`
挡住，所以克隆下来是没有的）：

| 脚本 | 用途 | 要点 |
| :--- | :--- | :--- |
| `build/poll_ci.py <sha>` | 轮询某个提交的 check-runs | 必须**解析 JSON**，不能 `grep '"id"'`（嵌套对象也有 `id`，会多数） |
| `build/make_release.py <tag> <body.md>` | 建 Release | 要**显式传标题**；正文取自手写的英文 `.md` |

两个坑：

- **作业日志与 `GITHUB_STEP_SUMMARY` 需要登录才能读**（匿名访问 `/actions/jobs/<id>/logs` 回 `403`），
  所以 CI 摘要里的内容（例如 clang-tidy 的路径清单）**在本地读不到**，要么在浏览器里看，要么在
  workflow 里把它 `echo` 进日志并 `upload-artifact`；
- 取作业日志的请求会 302 到 blob 存储，`urllib` 会把 `Authorization` 一起带过去并得到
  `403 signature` 错误——需要在跨域重定向时**剥掉这个头**。

---

## 七、完成后

1. 把「发布记录」写进本版的设计文档（`doc/design/<ver>-TASKS.md` 的执行记录）：闸门结果、tag 的
   **提交与 tag 对象两个 SHA**、Release 标题、版本结算的结论；
2. 确认工作区干净（`git status --short` 为空）；
3. 两端 `git ls-remote master` 与本地 `git rev-parse master` 三者一致。
