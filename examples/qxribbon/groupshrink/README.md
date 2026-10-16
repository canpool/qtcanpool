演示 4.5 的组级缩放（B11）：窗口变窄时，Ribbon 页会**先从右往左**把各组切成 reduced（小按钮）形态，缩不动时才出现页级滚动条。

- 每个组都放了至少一个大按钮（独占一列），所以都有 reduced 形态可供切换。
- 状态栏实时显示每组的 `RibbonGroup::isReduced()` 状态：把窗口拖窄，可见各组从右侧依次变成 `reduced`，全部缩到最小仍放不下才出现左右滚动按钮。
- `RibbonGroup::setReduced()/isReduced()` 是给「组不在页面里」的宿主用的 API；页面内的组由 `RibbonPage` 在每次布局时自行决定，本例用 `isReduced()` 把这一决定显示出来。
