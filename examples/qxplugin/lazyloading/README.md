演示 4.5 新增的**按需插件加载**：`QxPluginManager::resolvePlugins()` 读入并排好所有插件的元数据、
但一个都不启动；`QxPluginManager::requestPlugin()` 在宿主第一次要某个插件时，才把它以及它还缺的
依赖启动起来。

宿主因此可以**先把窗口拉起来**，用到哪个模块再为哪个模块付出代价，而不是在启动时把全部插件都加载一遍。
同一份元数据照样能驱动一次性的 `loadPlugins()` —— 当宿主更想一次全部启动时。

运行后输出（节选）：

```
after resolvePlugins(): started 0 plugin(s)
load order would be: a -> c -> b
requestPlugin("b") -> b
  lifecycle so far: a: initialized, b: initialized, a: extensions initialized, b: extensions initialized
requestPlugin("c") -> c
  lifecycle so far: a: initialized, b: initialized, a: extensions initialized, b: extensions initialized, c: initialized, c: extensions initialized
```

要点：`resolvePlugins()` 之后 `log` 为空（没有启动任何插件）；要 `b` 时先启动它依赖的 `a`，再启动 `b`；
之后单独要 `c`，已经启动过的 `a`/`b` 不会被再次启动，`c` 的 `extensionsInitialized()` 也不会让它们的重跑。
