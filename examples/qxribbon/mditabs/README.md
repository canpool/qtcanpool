# MDI Tabs

演示 4.5 新增的 `QxRibbon::RibbonMdiArea`：**标签式文档 MDI 区 + 文档页随活动文档切换**。

## 这个示例展示什么

- **文档以标签页呈现**（tabbed view + document mode，标签可关可拖）——这是 ribbon 应用
  呈现打开文档的方式；「Window」组的「Tabbed Documents」可切回经典子窗口形态。
- **切换标签 = 切换 ribbon 页**：每个文档在 `addDocument()` 时登记一个 `RibbonPage`，
  它是活动文档时该页在上，不是时收起。Home 页不登记给任何文档，因此始终保留——
  这正是文档类应用启动时的典型观感。
- **文档页作用于活动文档**：Text 页的 undo/redo/cut/copy/paste 直接作用于当前活动的
  编辑器——页面切换与"页操作当前文档"是同一机制的两面。
- **窗口动作**：Tile / Cascade / Close All（`QMdiArea` 的既有槽直接可用），状态栏实时显示
  活动文档与打开数量。

## 怎么试

1. 「File → New Text / New Sketch」各开几个文档，看顶部标签。
2. 点不同标签：ribbon 的 Text / Sketch 页跟着换，Home 页不动。
3. 关掉活动文档：它的页随之收起；全部关掉后只剩 Home 页。
4. 「Window → Tabbed Documents」取消勾选，回到经典 MDI 子窗口（此时 Tile / Cascade 才有直观意义）。

## 实现要点

- `RibbonMdiArea` 派生自 `QMdiArea`——Qt 的全部 MDI 能力（`addSubWindow`、`tileSubWindows`、
  `cascadeSubWindows`、视图模式……）原样可用，库补的是 ribbon 集成层。
- 页面必须在交给 `addDocument()` **之前**已加入 `RibbonBar`（库通过 `page->ribbonBar()`
  找到该和哪个条对话）；不带页面的文档不参与切换，不在任何条上的页面会被原样放过。
- 不变量：**登记的页面中，只有活动文档的那个在上；无活动文档时都不在上**。
- ⚠️ Qt 的一个已知行为：以标签模式打开的**第一个**文档，其标签虽呈选中态，
  `activeSubWindow()` 仍为 null，直接点这个已选中的标签不会有反应——访问过别的标签
  再点回来即恢复正常。详见 `doc/pages/limitations.md` 的对应条目。本示例在窗口显示前
  打开首个文档（显示动作会激活它），因此启动时看不到这个状态。
