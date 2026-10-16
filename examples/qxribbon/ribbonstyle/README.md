通过右上角 RibbonButtonGroup 配合 RibbonMenu 下拉切换 Ribbon 的两类外观：

- **布局样式**（Office / Wps / 两行）：`RibbonBar::setRibbonStyle()`。
- **配色主题**（Office 2013 / Fancy / Dark Office Plus / **Fluent 2016** / **Fluent 2019**）：`RibbonTheme::setTheme()`（经 `RibbonMainWindow::setRibbonTheme()`）。后两者为 4.5 新增（B7），沿用 Office 2013 选择器、调色板重映射到 Fluent 强调色/中性色，枚举追加在 `CustomTheme` 之后，既有取值不变。
