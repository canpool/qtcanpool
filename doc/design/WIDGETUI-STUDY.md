# fluent / material 作为 widget UI 库：可行性勘察

> **性质：选型勘察**。不动代码，不进任何版本的台账（`4.6-TASKS.md` §一 明写「选型勘察一律不写进
> 本文件」）。本文件只做一件事：把「要不要建、建什么、代价多少、和现有定位冲不冲突」摆清楚，
> 由作者据以定范围。
>
> **起因**：作者 2026-10-20 裁定「fluent 和 material 不是主题族，是开发 widget ui 库」，并把已经
> 提交的两族共 4 张表连同生成器接线整体撤回（`git reset --hard` 到 `d4b39127`，强推两远程）。
> 同日授权先做本勘察。
>
> **结论先行**（依据见下文各节）：
> 1. 两套设计语言都是**控件集**（~40–60 项），Qt 官方只在 **Qt Quick** 侧提供实现，**Qt Widgets
>    一个官方实现都没有**；第三方 Qt Widgets 实现里最成熟的几条，许可证对本项目**不可用或需评估**。
> 2. 本仓库现有 7 个库**没有任何一个通用输入控件**——只有 Ribbon / Dock / 应用外壳这些 chrome。
>    所以「控件库」不是把现有的东西改造，而是**从零起一个新面**。
> 3. ⚠️ **最该先说的一条**：Fluent 的官方导航范式（NavigationView + CommandBar）与 Material 的
>    （TopAppBar + NavigationRail + FAB）里**都没有 Ribbon 的位置**。做这两套控件集与 `qxribbon`
>    是**范式竞争**关系，不是互补。这一条比工作量更能决定要不要做。
> 4. 观感差距的大头**不在控件外形，在令牌轴**：现有 22 个令牌**全是颜色**，缺 shape / elevation /
>    type / spacing / motion 五条轴。只补颜色必然重演 4.5 已经付过学费的「四套亮和四套暗风格完全
>    一样」。
> 5. ⇒ 建议的次序是：**先把令牌轴补齐（小、可验收、不动控件）→ 再评估要不要 QStyle 原语 →
>    最后才谈新建控件库**。详见 §七。

---

## 一、两套设计语言各自的控件量级

### 1.1 Fluent（WinUI 3 / Fluent 2）

出处：Microsoft Learn《Controls for Windows apps》（2026-10-20 取），原文自述「over 45 ready-to-use
controls」，按 12 个类目列。**这是控件集，不是配色方案**——每一项都有自己的几何、状态层、动效与
键盘行为。

| 类目 | 控件 |
| :--- | :--- |
| 基本输入 | Buttons（button / drop-down button / split button / toggle button 等）、Check boxes、Combo boxes、Hyperlinks、Radio buttons、Rating control、Sliders、Toggle switches |
| 集合 | Items view、List view & grid view、Flip view、Pips pager、Tree view、Items repeater、Swipe、Pull-to-refresh |
| 对话框与浮出 | Dialogs、Flyouts、Popup、Teaching tip |
| 表单 | Forms（一种 pattern，不是单个控件） |
| 媒体 / 图形 / 形状 | Icons、Animated icons、Images & image brushes、Media playback、Shapes、Inking controls |
| 菜单与工具栏 | Menus & context menus、Command bar、Command bar flyout、Menu flyout & menu bar |
| 导航 | Breadcrumb bar、List/details、Navigation view、Pivot、Selector bar、Tab view |
| 人物 | Person picture |
| 选择器 | Color picker、Calendar date picker、Calendar view、Date picker、Time picker |
| 滚动与布局 | Expander、Scrolling & panning、Annotated scrollbar、Semantic zoom、Split view、Two-pane view |
| 状态与提示 | Progress、Tooltip、Info bar、Info badge |
| 标题栏 | Title bar |

### 1.2 Material 3（含 2026 的 M3 Expressive）

出处：`m3.material.io` 组件索引与 2026 年的 M3 Expressive 公告（该站正文由脚本渲染，抓取只能取到
标题，故分类以官方 glossary 页与 Expressive 公告的原文条目为准）。

| 类目 | 组件 |
| :--- | :--- |
| Actions | Buttons（filled / tonal / outlined / text / elevated）、FAB（+ extended）、Icon buttons、Segmented buttons、Button groups（Expressive 新增）、Split button（Expressive 新增） |
| Communication | Badges、Progress indicators（linear / circular）、Snackbars、Tooltips |
| Containment | Bottom sheets、Cards、Carousel、Dialogs、Dividers、Lists、Menus、Side sheets、Toolbars（Expressive 新增） |
| Navigation | Bottom app bar、Navigation bar、Navigation drawer、Navigation rail、Search、Tabs（primary / secondary）、Top app bar |
| Selection | Checkbox、Chips、Date pickers、Radio button、Sliders、Switch、Time pickers |
| Text inputs | Text fields |
| 数据 | Data tables |

M3 Expressive（2026）在组件之外还改了 **motion physics**（动效从 duration 曲线换成物理参数）与
**shape library**（35 种形状，带 shape morph 动效）——这两项恰好是本仓库完全没有的两条轴（§四）。

### 1.3 一条决定性的事实：官方实现只在 QML 侧

Qt 文档《Styling Qt Quick Controls》（6.8 版）列出的官方风格里，与两套设计语言直接对应的有：

- **Material**：Qt Quick Controls 自带（Qt 5.7 起）；
- **Universal**：基于微软 Universal Design Guidelines（UWP 时代，不是 Fluent 2）；
- **FluentWinUI3**：Qt **6.8** 起提供，原文「遵循 Fluent UI 和 WinUI 3 设计指南，可在所有支持的平台上运行」。

**Qt Widgets 侧一个都没有。** 这不是本项目的疏忽，是 Qt 的路线选择：设计语言级的观感由
Qt Quick Controls 的 style 机制承担，Widgets 只提供平台原生 style（Windows / Fusion / macOS）。

第三方现状（2026-10-20 检索）：

| 项目 | 技术路线 | 许可证 | 可评估性 |
| :--- | :--- | :--- | :--- |
| `zhuzichu520/FluentUI` | QML 原生控件 | MIT | 只对 QML 有用，与 Widgets 无关 |
| `zhiyiYo/PyQt-Fluent-Widgets` | Widgets 控件库（Python 绑定） | **GPL + 商业双授权** | ⚠️ GPL 与本项目 MulanPSL-2.0 **不兼容**，不可用 |
| `mowangshuying/CppQtFluentUi888` | Widgets「样式化」路线（C++） | LGPL-2.1 | 可参考，但路线＝大规模 QSS，与本仓库现状同形 |
| `githubuser0xFFFF/QtFluentDesign` | Widgets 样式化 | LGPL | 同上 |
| `laserpants/qt-material-widgets` | Widgets 控件库 | BSD | 多年不活跃 |
| `UN-GCPDS/qt-material` | **只是主题**（PySide/PyQt） | BSD | 不是控件集 |

⇒ 两条可用信号：
① **Widgets 侧做得成的成熟实现，主流都是「样式化」路线**（＝把 QSS 写到极致的形态），而不是
「每个控件自绘一遍」。这与本仓库 `qxtheme` 的路线同族。
② 真正「控件库」形态的实现要么是 GPL（不可用）、要么停更。

---

## 二、与本仓库现有面的对照

### 2.1 现有控件清单（2026-10-20 实测，`grep` 导出类）

| 库 | 导出行 | 其中 QWidget 系 | 说明 |
| :--- | ---: | ---: | :--- |
| qxribbon | 30 | 25 | RibbonBar / RibbonPage / RibbonGroup / RibbonButton / RibbonGallery / RibbonBackstageView / RibbonQuickAccessBar / RibbonStatusBar / WindowButtonGroup … |
| qxdock | 28 | 21 | DockWidget / DockPanel / DockTabBar / DockTitleBar / DockSideBar / DockAutoHideContainer / DockOverlay … |
| qxapp | 18 | 15 | QxAppShell / QxNavigationBar / QxTabBar / QxTabWidget / QxToast / QxPropertyEditor / QxSettingsDialog … |
| qxwindow | 2 | 0 | 两个 agent（无边框窗口的命中测试与拖拽），材质由内部私有类实现 |
| qxcore / qxtheme / qxplugin | 2 / 2 / 3 | 0 | 非可视 |

（「导出行」是 `QX_*_EXPORT` 声明行数，含少量无基类的结构体声明如 `RibbonStyleOption`、
`DockStyleColors`；QWidget 系按基类链逐个判定。）

**关键读数：这 61 个 QWidget 系类，没有一个是你会在普通表单里用的输入控件。**
全是「容器 / chrome / 外壳」——Ribbon 的栏与组、Dock 的面板与页签、应用的导航与提示。
普通控件（按钮、勾选、下拉、滑杆、输入框）在本仓库里**一律是 Qt 原生类 + QSS 上色**。

### 2.2 重叠面：只有结构对得上，没有控件对得上

| 设计语言里的项 | 本仓库现状 | 判定 |
| :--- | :--- | :--- |
| Tab view / Tabs | `DockTabBar` / `DockTab` / `QxTabBar` / `QxTabWidget` | 结构对应，**造型是自成一族**（4.5 的四家族各有自己的页签结构） |
| Navigation view / Navigation rail | `QxNavigationBar`（qxapp） | 部分对应，且是 Ribbon 的左侧栏，不是 Fluent 的 NavigationView |
| Info bar / Snackbar | `QxToast`（qxapp） | 部分对应（有提示，无「内联消息条 + 关闭 + 动作按钮」的完整形态） |
| Split view | dock 的面板与分割器 | 结构对应，但 docking 是超集，不是同一个东西 |
| Menu bar / Command bar | `RibbonBar`（`QMenuBar` 子类）/ `RibbonQuickAccessBar` | 结构对应，**范式不同**（见 §2.4） |
| Title bar | `qxwindow` 的无边框接管 + 窗口材质 | 对应度高，且**是本仓库少见的领先项**（Mica / Acrylic / Mica Alt 都有实现） |
| Person picture / Breadcrumb / Rating / Info badge / Chips / Switch / Segmented / Card / FAB / Progress ring | 无 | 缺口 |

### 2.3 Qt Widgets 缺的控件（缺口清单）

Qt Widgets 提供的通用控件：按钮 / 工具按钮 / 单选 / 勾选 / 下拉 / 行编辑 / 文本编辑 / 数值框 /
滑杆 / 滚动条 / 进度条 / 标签 / 视图（list·tree·table）× 3 / 页签 / 菜单 / 工具条 / 状态条 /
分割器 / 分组框 / 对话框 + 一批标准对话框 / 日历 / 日期时间 / 表盘。

两套设计语言要求、而 Qt Widgets **没有**的：

`ToggleSwitch`（Qt 只有 QCheckBox）· `Chip` · `SegmentedButton` · `ButtonGroup`（几何联动）·
`SplitButton` / `DropDownButton`（QToolButton 有菜单但无分体造型）· `Badge` · `InfoBar` ·
`Snackbar` · `Card` · `Rating` · `Breadcrumb` · `NavigationRail` / `NavigationView` · `FAB` ·
`ProgressRing` · `TeachingTip` / `Flyout`（Qt 只有 QToolTip）· `Avatar` / `PersonPicture` ·
`Skeleton`（加载占位）· `Expander`（`QToolBox` 是单开手风琴，不是可嵌套的 Expander）·
`Carousel` · `AnimatedIcon`

⇒ 这不是「差几个」，是**差一层**。

### 2.4 ⚠️ 结构冲突：Fluent / Material 的导航范式会取代 Ribbon

- WinUI 3 的标准应用骨架是 **NavigationView（左栏）+ CommandBar（上下文命令）+ TabView（文档）**；
  Fluent 的控件清单里**根本没有 Ribbon 这一项**（§1.1 表里与之同位的只有 Command bar 与
  Menu flyout & menu bar）。
- Material 的骨架是 **TopAppBar + NavigationRail/Drawer + FAB + BottomAppBar**；也没有 Ribbon。

而本项目的定位（`ROADMAP-3.x.md` §3.1、`architecture.md`）是 **Ribbon + Docking 的 IDE 外壳**，
`qxribbon` 是最大的两块资产之一（≈17k 行）。⇒ **做 fluent/material 控件库，等于给本项目引入一条
与自有资产竞争的技术路线。** 这条不解决，「做什么控件」都无从谈起——它不是工作量问题，是定位问题。

反过来，如果目标是「让现有的 Ribbon/Dock 应用看起来更现代」，那要的是**令牌与造型的现代化**
（更克制的边框、更明确的层级、动效），不是换一套控件范式。

---

## 三、三条实现路径的取舍

### 3.1 现状的观感体系是**三层**的

实测（2026-10-20）：

1. **QPalette**：`QxThemeManager::apply()` 先 `app->setPalette(paletteFor(dark))`，并把 accent 写进
   `Highlight` / `HighlightedText` / `Accent`；
2. **QSS 表**：紧接着 `app->setStyleSheet(info.styleSheet)`——`tools/themes/` 下 2 套骨架 + 5 份
   chrome + 1 份 menu 作模板，`build_themes.py` 代入令牌生成 5 族 × 亮暗 = **10 张表**（每张千行级）；
3. **自绘 chrome**：13 个 .cpp 覆写 `paintEvent`——其中 9 个直接读 `palette()` 取色，
   5 个改用 `style()->drawPrimitive/drawControl` 的**原语**（原语内部按 `option.palette` 取色，
   见 `ribbonbutton.cpp`、`qxtoolbutton.cpp`、`dockbutton.cpp`）。

第 3 层是关键事实：**凡是造型重要的地方，本仓库已经是自绘 + 读 palette**，而不是靠 QSS。
`RibbonGalleryGroup` 在 4.5 C1i 从 `QListView` 改成自绘 `QWidget`，理由正是「QSS 表达不了状态只改
洗色与描边、几何不变」——这是「QSS 的天花板在哪」最直接的一次实测。

### 3.2 路径 A：QSS-only（现状的延长）

- **能做**：颜色、边框、圆角、内边距、字体粗细、伪状态（hover / pressed / checked / disabled / open）、
  简单背景图（可做「看起来像 switch」的静态开关）。
- **做不到**：状态之间的**过渡**（Qt 样式表无 transition）、阴影（无 `box-shadow`）、
  拇指跟随的动画（QCheckBox 的 indicator 是画上去的，没有可插值的中间态）、
  真正的分层（elevation 要靠多层半透明叠加，QSS 只有一层背景）、
  自定义命中区域之外的行为（Chips 的删除按钮、Segmented 的滑动指示条）。
- **证据**：现有 10 张表已经写到千行级，`skeleton_dark.css` 已过千行——**继续往上堆的边际收益很低**，
  4.5 的 C1f/C1g/C1h 三轮修补都在这条路上。

### 3.3 路径 B：QStyle 子类 / proxy

- **唯一能一次覆盖所有 Qt 控件的办法**：写一个 `QProxyStyle`，把 `PE_` / `CE_` / `CC_` 原语按设计
  语言画出来，所有 `QPushButton`/`QCheckBox`/`QSlider`/`QComboBox` 自动变样，不用改一行业务代码。
- **代价与限制**：
  - 原语粒度很粗（`PE_PanelButtonCommand` 只给你一个 rect 和一个 state），设计语言里的
    「状态层 + 描边 + 焦点环 + 图标槽」要靠原语自己拆，仍要写不少几何；
  - **与 QSS 抢所有权**：`QStyleSheetStyle` 会代理当前应用 style，凡是 QSS 写了规则的属性就由 QSS
    说了算，没写的才回落到我们的 style ⇒ 一个控件的外观会有**两处真相**。本仓库的主题引擎就是
    QSS ⇒ 必须**切分边界**（例如：style 负责原语几何与状态层，QSS 只负责颜色令牌），否则每加一条
    QSS 规则就在悄悄夺走一次控制权；
  - Qt 5 与 Qt 6 的原语集合有差异，双腿都要过；
  - 无障碍：走 style 路线时 `QAccessible` 仍由 Qt 控件自己提供，这块**免费**——这是路线 B 相对
    路线 C 的最大优势。
- **业界先例**：Qt 自家的 Fusion 就是这条路；第三方 Widgets 的「样式化」实现（§1.3 表里两条
  LGPL 的）也多为这条路或其简化版。

### 3.4 路径 C：自绘控件集

- **保真度最高**，可以做到和设计语言像素级一致，动效也能自由实现。
- **代价**：每一个控件都要重新实现 Qt 已经做好的东西——输入法（IME）、输入校验、键盘导航与
  Tab 序、焦点环、`QAccessible` 接口、HiDPI 下的度量、RTL 镜像、拖放、上下文菜单。
  一个「看起来只是换了个样子」的开关，实际要维护的是这十项行为。
- **先例**：`RibbonButton` 就是这个模式，它的 `paintEvent` 已经长到需要分「大按钮/小按钮/
  菜单按钮」三条分支——单个控件就这个体量。

### 3.5 结论

**混合，且顺序必须是「先令牌轴 → 再原语 → 最后控件」**：
观感差距的大头是 shape / type / spacing / elevation / motion 这几条轴（§四）。前三条是**纯令牌**
（改 CSS 与生成器，不碰控件、不碰库）；elevation 与 motion 要各带一个**最小机制**（阴影的画法、
动效的雏形），但都还在 `qxtheme` 这一层，不需要新控件、不需要新库。等这几轴有了，
「还要不要 QStyle / 还要不要新控件」的答案会清楚得多——很可能大部分诉求已经解决。

---

## 四、令牌缺口（量化）

现有令牌（唯一事实源：`tools/themes/skeleton_*.css` 与 `chrome_*.css` 里的 `{{...}}` 占位符）共 22 个，
**全部是颜色**：

`surface` `surface-alt` `surface-sub` `border` `border-strong` `text` `text-muted` `body`
`text-on-fill` `accent` `accent-soft` `accent-faint` `accent-text` `accent-fill` `accent-fill-strong`
`state-rest` `state-hover` `state-pressed` `state-pressed-deep` `close-hover` `close-pressed`
`highlight`

缺的五条轴：

| 轴 | 现状 | 两套设计语言对它的依赖 |
| :--- | :--- | :--- |
| **shape**（圆角尺度） | 没有令牌，33 处 `border-radius` 直接写死，取值只有 `0 / 2 / 3 / 6px` 四档 | M3 有 7 级 corner scale，Expressive 又加 35 种形状；Fluent 是 4/8px 两档。**这是家族差异最直观的落点** |
| **elevation**（阴影层） | 无（QSS 无 `box-shadow`） | M3 五级 elevation 靠阴影；Fluent 靠 Mica/Acrylic 分层。QWidgets 只能用 `QGraphicsDropShadowEffect`（每实例一个 effect，开销大）或自绘 |
| **type**（字阶） | 近乎空白：模板里**零条** `font-weight` / `font-family`，`font-size` 只有 3 条（10 / 13 / 15px），**且只在暗色骨架里**——亮色表一条都没有 | M3 15 个 type role；Fluent 有 Type Ramp。现在没有任何字阶令牌，亮暗两侧还不对称 |
| **spacing**（密度） | 硬编码 padding | 两套都靠 4/8 网格；没有令牌就无法做「紧凑/舒适」两档密度 |
| **motion** | 全库**只有 2 个文件**用 `QPropertyAnimation`（`qxtoast.cpp`、`dockautohidecontainer.cpp`） | Fluent 的 reveal / connected animation、M3 的 motion physics 都是核心。**动效基建几乎为零** |

⚠️ 这条与 4.5 的教训直接呼应：当时「四套亮和四套暗风格完全一样」，根因是**只有色板不同、
结构没差异**；后来靠「四轴判据」（条与页的明暗 / 选中态 / 分隔与盒子 / 文件按钮）才拉开。
同理，**只加一套 fluent 配色、不加 shape/elevation/motion，一定重演同一个病**——这就是撤回那两族
在技术上的正当理由。

---

## 五、工作量（用本仓库的单位衡量）

| 参照物 | 规模（2026-10-18 实测，量级） |
| :--- | :--- |
| `qxribbon`（一个完整 chrome 库） | ≈17k 行 |
| `qxdock`（一个完整 chrome 库） | ≈17.5k 行 |
| `qxapp`（应用外壳） | ≈8k 行 |
| `qxtheme`（引擎 + 令牌） | ≈1.1k 行 |
| 主题 QSS 层（模板 → 10 张表） | 模板 2 千行级，产出万行级 |

三档估算（只含库代码 + 测试，不含文档与示例）：

| 范围 | 内容 | 量级 | 说明 |
| :--- | :--- | :--- | :--- |
| **S · 令牌轴补齐** | shape / elevation / type / spacing / motion 五轴进 `qxtheme`，10 张表重生成 | 数千行（模板为主） | 不新增控件、不新增库；验收＝像素探针逐轴比对 |
| **M · QStyle 风格库** | 一个新库：`QProxyStyle` 子类 × 2 套设计语言 + 原语几何 + 与 QSS 的边界规则 | 数万行 | 覆盖所有 Qt 控件，无障碍免费；但两处真相要长期维护 |
| **L · 自绘控件库** | 12–20 个 Qt 缺失控件（switch / chip / segmented / card / infobar / badge / rating / breadcrumb / rail / FAB / ring / skeleton …） | 数万行，**接近再造一个 `qxribbon`** | 每个控件都要重做 IME / 键盘 / 无障碍 / RTL / HiDPI |

⇒ 一句话：**S 是能在一版内交付的；M 需要一版专门做它；L 一版开不了头**。

---

## 六、风险清单

1. **定位冲突（最高）**：4.6 的「做深判据」把「加新类 / 新控件 / 新库」明确列为**反例**。
   任何「建控件库」的选项都要先由作者改定位，或落到允许破例的版本上。
2. **范式竞争（最高）**：见 §2.4——Fluent / Material 的导航范式与 Ribbon 互斥。
3. **与 QSS 主题引擎抢所有权（高）**：本仓库的主题就是 QSS；引入 QStyle 必须切分边界，
   否则同一处外观两处真相（4.5 已经吃过「同一答案在两处各算一遍」的苦）。
4. **双 Qt 腿（中）**：Qt 5 与 Qt 6 的 `QStyle` 原语、`QPalette` 角色（`Accent` 是 6.x 才有）、
   高 DPI 度量都有差异；`qxwindow`/`qxribbon` 已经证明双腿各自都有独有的坑。
5. **无障碍（中）**：自绘控件必须补 `QAccessible*`，否则屏幕阅读器与键盘访问退化；
   style 路线免费，控件路线要逐个做。
6. **许可证（中）**：最成熟的 Widgets Fluent 实现是 GPL+商业双授权，与 MulanPSL-2.0 不兼容；
   可参考的 LGPL 实现要评估传染边界。
7. **设计语言本身在演进（中）**：M3 Expressive（2026）刚改过一轮动效与形状；跟版是长期成本。
8. **命名（低但需定）**：仓库硬规则「公共 API / 枚举 / 主题 id 按**形态或几何**命名，不用产品名」。
   库名与对外标识若叫 `fluent` / `material`，是否算破例，需要作者一句话。

**唯一的现成协同点**：Fluent 的窗口材质（Mica / Acrylic / Mica Alt）在 `qxwindow` **已有实现**
（`windowkit_win_p.h` 里三档 `_DWMSBT_*` 都在用）。这是本仓库在这一方向上少见的领先项，
也是「做 Fluent 观感」时唯一不需要新建的地基。

---

## 七、给作者定的范围选项

| 选项 | 做什么 | 交付判据 | 风险 |
| :--- | :--- | :--- | :--- |
| **A · 只定口径，不建库** | 在设计文档里记下「fluent / material 是控件集，不是主题族」，主题族继续按形态命名 | 口径入档，无代码 | 零风险；问题原地保留 |
| **B · 先补令牌轴（建议）** | 在 `qxtheme` 里增 shape / elevation / type / spacing / motion 五轴令牌，10 张表重生成；不新增控件 | 每轴一个像素探针 + 逐表同色/同几何比对 | 需要 `tools/themes` 生成器扩展；elevation 在 QWidgets 里没有现成手段（要定方案） |
| **C · 立项 QStyle 风格库** | 新库：两套设计语言的 `QProxyStyle` + 原语几何 + 与 QSS 的边界契约 | 所有 Qt 控件自动变样；无障碍免费；双 Qt 腿通过 | 与既有 QSS 主题抢所有权；需改 4.6「不加新库」定位 |
| **D · 立项自绘控件库** | 新库：只做 Qt 缺的控件，风格由令牌驱动 | 按控件逐个验收（像素 + 交互 + 无障碍） | 工作量≈再造一个 `qxribbon`；与 Ribbon 范式竞争 |
| **E · 混合（C + D）** | style 管原语，控件库管 Qt 没有的件 | 同上两者的合集 | 工作量最大，两套系统要长期共存 |

**建议路径**：**A + B 先行，C/D 的决策后置**。理由：
- B 是唯一「不动控件、不动定位、可逐条验收」的选项，且正好补的是「只加颜色必然重演四套一样」的病根；
- B 做完之后，「观感还差多少」会变成一个可测量的问题（像素探针比对），而不是感觉问题；
- C/D 的取舍那时才需要作者拍定位——现在拍，信息不足。

⚠️ 若最终选 C 或 D，还需同时决定两件事：**版本归属**（4.6 的判据不允许加新库，需另择版本）
与**命名口径**（库名能否用设计语言名，见 §六.8）。

---

## 八、若将来立项：最小可信的第一批

按「Qt 完全没有 → 用得上 → 能验收」排序，12 项以内：

1. `Switch`（Qt 只有 QCheckBox，两套设计语言都把它当一等公民）
2. `SegmentedButton`（M3 有、Fluent 的 Selector bar 同形）
3. `Chip`（assist / filter 两种即可）
4. `InfoBar`（内联消息条：图标 + 文本 + 动作 + 关闭）
5. `Badge`（数字 + 圆点两形态）
6. `Card`（contained / elevated 两档，直接吃 elevation 令牌）
7. `BreadcrumbBar`（截断与溢出菜单是难点）
8. `Rating`（半星 + 键盘）
9. `ProgressRing`（QProgressBar 只有线性）
10. `Skeleton`（加载占位，纯绘制、成本低、观感提升明显）
11. `Expander`（可嵌套，补 QToolBox 的不足）
12. `Flyout / TeachingTip`（QToolTip 是只读的，缺带交互的浮层）

`Switch` 与 `Chip` 建议作为**第一个**：它们能一次把「令牌 → 几何 → 状态层 → 动效 →
无障碍 → 回归通道」整条链路跑通，是整条路的样板。

---

## 九、勘察方法与证据出处

- 控件清单：`grep` 导出类（`^class QX_.._EXPORT .*: public`）+ 递归统计；数字见 §2.1。
- 自绘面积：`grep -rl '::paintEvent(QPaintEvent' --include=*.cpp src/` → 13 个文件；其中
  9 个出现 `palette()` / `QPalette`，5 个出现 `drawPrimitive|drawControl`。
- 动效基建：`grep -rl QPropertyAnimation\|QVariantAnimation\|QAbstractAnimation src/libs/` → 2 个文件。
- 令牌：`grep -o '{{[a-z0-9-]*}}' tools/themes/*.css | sort -u` → 22 个，全为颜色。
- 圆角：`grep -o 'border-radius: [0-9]*px' tools/themes/*.css | sort | uniq -c` → `0px`×17、
  `2px`×10、`3px`×4、`6px`×2（共 33 处，无令牌）。
- 字阶：`grep -c 'font-weight\|font-size\|font-family' tools/themes/*.css` → 全部为 0，除
  `skeleton_dark.css` 的 3 条 `font-size`；对应生成表里 `*_dark.css` 各 3 条、`*_light.css` 各 0 条。
- 主题下装：`qxthememanager.cpp` 的 `apply()`（`setPalette` + `setStyleSheet`）。
- 窗口材质：`windowkit_win_p.h` 的 `_DWMSBT_MAINWINDOW` / `_TRANSIENTWINDOW` / `_TABBEDWINDOW`。
- 设计语言清单：Microsoft Learn《Controls for Windows apps》；`m3.material.io` 组件索引与
  M3 Expressive 公告；Qt《Styling Qt Quick Controls》6.8 版。
- 第三方现状：各项目 GitHub 主页与一篇 2024 年的 Qt 控件库综述（许可证以项目主页为准）。

---

## 十、待作者回答的三个问题

1. **目标是哪个？**「让现有 Ribbon/Dock 应用更现代」还是「另立一套 fluent/material 形态的应用」？
   前者指向 B，后者才指向 C/D。
2. **若做控件库，版本落在哪里？** 4.6 的判据不允许加新库，需要另择版本或改定位。
3. **命名口径**：库名与对外标识能否用设计语言名（`fluent` / `material`），还是同样要按形态命名？
