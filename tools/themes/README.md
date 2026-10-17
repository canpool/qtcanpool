# 主题生成

`res/themes/*.css` 是**生成物**，不要手改 —— 跑一遍脚本就会被覆盖。
一套主题由三部分相加而成：

| 文件 | 作用 |
| :--- | :--- |
| `skeleton_light.css` | 浅色骨架：只描述 Ribbon 部件。标准控件交给平台样式 + 浅色调色板 |
| `skeleton_dark.css` | 暗色骨架：除 Ribbon 外**还要重画标准控件**（暗色下平台样式会忽略调色板，否则下拉框、树、滚动条仍是浅色） |
| `chrome_<family>.css` | 家族的**结构**差异：页签怎么表示选中、圆角、页签条与页面之间有没有分隔线、强调色用在哪 |
| `build_themes.py` 里的 `PALETTES` | 该家族的**颜色**角色 |

三者拼起来 = `res/themes/<family>_<scheme>.css`。现在有 5 个家族（classic / soft /
flat / slate / accent）× 明暗两套 = 10 个文件。

> 只换颜色不足以成为一个家族。几套主题如果共用同一份骨架、只改色板，选相邻一档的
> 深灰看起来就是"完全一样" —— 这正是为什么要单独有 `chrome_*.css`。五个家族在
> **四条轴**上同时不同，所以不看名字也能分：
>
> | | 条与页的明暗 | 选中态 | 分隔与盒子 | 文件按钮 |
> | :--- | :--- | :--- | :--- | :--- |
> | classic | 条灰、页白 | 抬高的白卡片（直角、带边） | 分组竖线 + 大按钮有框 | 实心强调色方块 |
> | soft | 条白、页灰（反过来的） | 开进正文的圆角开口（无边框） | 全无 | 实心强调色、圆角 |
> | flat | 同色，一层 | 强调色 2px 下划线 + 文字取强调色 | 全无 | 实心强调色方块 |
> | slate | 条深灰、页白 | 留在条上，只加顶部 1px 强调线 | 分组发丝线、无框 | 无填充，仅强调色文字 + 下划线 |
> | accent | 条白、页白 | 强调色**实填**选中标签（白字） | 全无 | 实心强调色方块 |

## 做一套新主题

1. **定家族名**，例如 `acme`。把 `chrome_acme.css` 放在本目录，只写这个家族
   与其它家族**不同**的地方（选中态、圆角、分隔线）。没写的部分沿用骨架。
2. 在 `build_themes.py` 里加两条调色板：`("acme", "light")` 与 `("acme", "dark")`，
   把下表的角色填满（每个 palette 都要填全，否则脚本会报缺少哪个角色）。
3. 把 `acme` 加进 `FAMILIES`，以及 `FAMILY_NOTES`（会写进生成文件的头注释）。
4. 跑：

   ```bash
   python3 tools/themes/build_themes.py
   ```

   然后 `git diff res/themes/` 复核，把生成的文件一起提交。

5. 要让应用能用：`res/themes.qrc` 里加条目，应用侧 `QxThemeManager::registerTheme()`
   给一个 id（约定 `<family>-<scheme>`，如 `acme-dark`，`dark` 传 `true`）。

## 颜色角色

| 角色 | 用在哪 |
| :--- | :--- |
| `text` / `text-muted` / `text-on-fill` | 主文字 / 次要文字 / 压在强调色块上的文字 |
| `surface` / `surface-alt` / `surface-sub` | Ribbon 条（chrome，页签坐在上面）/ 次级面 / 更次一级（子控件） |
| `body` | **正文档位**：页签下方的 ribbon 页面。与 `surface` 分开，才可能让某些家族"条浅页深"、另一些"条深页浅" |
| `state-hover` / `state-pressed` / `state-pressed-deep` | 悬停 / 按下 / 按到底 |
| `border` / `border-strong` | 发丝线 / 需要看清的边（分隔线、输入框边） |
| `accent` / `accent-hover` | 强调色：焦点边、下划线、选中文字 |
| `accent-fill` / `accent-fill-strong` | 强调色**填充**面（选中项底、标题栏按钮悬停） |
| `accent-faint` / `accent-soft` | 强调色的极淡/淡色洗（悬停底色、淡边框） |
| `highlight` | 提示性黄（下拉框悬停边） |
| `close-hover` / `close-pressed` | 关闭按钮悬停/按下（微软系红，通常不改） |

## 两条约定

1. **样式表里不写图标**。Ribbon 的箭头、窗口按钮、对话框启动器都是单色掩膜，
   颜色由 `QPalette` 决定（`qxribbon` 用 `QxRibbon::tintedIcon()` 画），换主题时自动重绘。
2. 调色板只由**明暗**决定（`QxTheme::paletteFor(bool)`），所以一个家族只要给对
   `dark` 就不用碰库；样式表负责"这套设计语言长什么样"，调色板负责"没被样式表
   描述到的控件取什么色"。
