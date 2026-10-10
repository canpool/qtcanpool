# 插件体系

QtCanpool 的插件体系把**应用**与**它由哪些模块组成**分开：应用只认识框架
（@ref QxApp::QxAppShell + @ref QxPlugin::QxPluginManager），模块以插件形式在运行时被发现、
排序、加载。一个模块可以单独编译、单独测试、单独启用或停用，而宿主源码里不需要出现任何一个模块名。

> 本页是插件开发指南。框架实现在 `src/libs/qxplugin`（契约 / 元数据 / 管理器 / 对象池）与
> `src/libs/qxapp`（@ref QxApp::QxPluginManagerDialog）；"不认识任何模块"的可运行样板在
> `demos/qxapp/ideshell`，最小的三个模块在 `src/modules/`。

## 一个插件是什么

一个插件就是一个 `QObject`，派生自 @ref QxPlugin::QxPlugin，并在声明里带上元数据：

```cpp
#include "qxplugin/qxplugin.h"

class OutputPlugin : public QxPlugin::QxPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QX_PLUGIN_IID FILE "output.json")
public:
    bool initialize(::QxPlugin::QxPluginContext *context, QString *errorString) override;
    void shutdown() override;
};
```

要点：

- **IID 是入场券**：`QX_PLUGIN_IID`（当前为 `"org.qtcanpool.QtCanpool.QxPlugin/1.0"`）写进
  `Q_PLUGIN_METADATA`，`QPluginLoader` 与管理器据此认定"这是我们的插件"；它同时是**接口版本**的锚点。
- **`FILE "output.json"` 指的是构建产物里的那份元数据**（由源码目录里的 `.json.in` 模板生成，见下），
  不是模板本身。
- ⚠️ **派生类里的 `QxPlugin::` 会被类名遮蔽**：`QxPlugin` 既是命名空间名又是基类名，在派生类里裸写
  `QxPlugin::QxPluginContext` 会去找**类成员**而非命名空间。**加前置 `::`**：`::QxPlugin::QxPluginContext`。
  `src/modules/` 里每个模块都因此这么写。
- **刻意没有 `Q_DECLARE_INTERFACE`**：`QxPlugin` 是 `QObject` 派生类而非纯接口，`qobject_cast` 走元对象
  链（任意继承深度都成立）。若把 `QxPlugin` 声明为 Qt 接口，`qobject_cast` 会改走 `qt_metacast(IID)`，
  而那要求**每个插件自己再写 `Q_INTERFACES(QxPlugin)`**——`moc` 不继承这张表，于是"照文档写的插件能加载
  却永远 cast 不上"。把插件与库绑在一起的是 `Q_PLUGIN_METADATA` 里的 IID，那也正是加载器检查的东西。

## 元数据：plugin.json

每个插件带一份 JSON。`src/modules/output/output.json.in` 是完整样例。读取实现在
@ref QxPlugin::QxPluginSpec 的 `read()`，字段如下：

| 字段 | 必填 | 说明 |
| :--- | :--- | :--- |
| `Name` | ✅ | 插件 id，**必须唯一**；也是库文件与元数据文件的主名。缺了 `read()` 直接失败 |
| `Version` | | 插件自身版本；缺省或不可解析时为 `1.0.0` |
| `CompatVersion` | | 仍然支持的最低宿主版本，宿主版本须落在 `[CompatVersion, Version]`；缺省时取 `Version` |
| `Vendor` / `Copyright` / `Category` / `Description` / `Url` | | 展示与诊断用 |
| `EnabledByDefault` | | false 表示默认不启动，除非被显式启用（缺省为 true） |
| `Dependencies` | | 依赖数组，见下 |

`Dependencies` 的每一项：

```json
"Dependencies" : [
    { "Name" : "output",     "Version" : "4.0.0" },
    { "Name" : "someother",  "Version" : "4.0.0", "Type" : "optional" },
    { "Name" : "testhelper", "Version" : "4.0.0", "Type" : "test" }
]
```

`Type` 缺省（或写成 `"required"`）即**硬依赖**；`optional` / `test` 的语义见「三种依赖」。

> **这份文件通常不用手写**：`add_qtc_plugin()` 从 `.json.in` 模板生成它，并把
> `PLUGIN_DEPENDS` / `PLUGIN_RECOMMENDS` / `PLUGIN_TEST_DEPENDS` 折成上面的 `Dependencies` 数组。
> 需要手写的场合是外部工程（见「在仓库外构建插件」）。

## 构建：add_qtc_plugin()

```cmake
add_qtc_plugin(filetree
  PLUGIN_CLASS FileTreePlugin
  DEPENDS qxplugin Qt5::Core Qt5::Gui Qt5::Widgets
  PLUGIN_RECOMMENDS output
  SOURCES
    filetreeplugin.cpp filetreeplugin.h
)
```

| 参数 | 作用 |
| :--- | :--- |
| `PLUGIN_CLASS` | `Q_PLUGIN_METADATA` 所在的那个类 |
| `DEPENDS` | **链接**依赖（这里是 `qxplugin` 与 Qt 模块） |
| `PLUGIN_DEPENDS` | 写进元数据的 **required** 依赖；同时按 PRIVATE 链接被依赖的插件目标 |
| `PLUGIN_RECOMMENDS` | 写进元数据的 **optional** 依赖（`"Type" : "optional"`） |
| `PLUGIN_TEST_DEPENDS` | 写进元数据的 **test** 依赖（`"Type" : "test"`） |
| `SOURCES` | 源文件 |

- **`DEPENDS` 与 `PLUGIN_*` 不是一回事**：前者决定编译 / 链接，后者决定**元数据与加载顺序**。
  一个模块可以链接某库却不在元数据里声明它（例如只链接 `qxplugin`）。
- **约定：目标名 = 插件 id = 元数据文件主名**，且源码目录里有同名 `*.json.in`。
- `add_qtc_plugin()` 还负责：生成 `plugin.json`、把目标安装到 `IDE_PLUGIN_PATH`、在
  `QTC_STATIC_BUILD`（含 wasm 预设）下改产**静态库**（见「静态构建」）。

## 生命周期

管理器按下面的顺序驱动一个插件，@ref QxPlugin::QxPluginState 记录它当前处于哪一步：

```
Read → Resolved → Loaded → Initialized →（宿主关闭时）Stopped
```

| 回调 | 何时被调用 | 该做什么 |
| :--- | :--- | :--- |
| `initialize(context, errorString)` | 所有 **required** 依赖都初始化完之后 | 取用 `context` 的能力、建自己的部件、把要给别人用的东西**发布进对象池**。返回 `false`（并填 `errorString`）表示"我起不来" |
| `extensionsInitialized()` | **全部**插件都 initialize 完之后，按**初始化顺序** | 需要"等整套就绪"的收尾。peer 在这里与 `initialize()` 里一样可达；但**没有** Qt Creator 那种逆序保证 |
| `shutdown()` | 宿主关闭时，按**初始化逆序** | 释放资源、断开连接。**不可失败** |

要点：

- **在 `initialize()` 里接线，不要等到 `extensionsInitialized()`**：消费方向（"我要连上游"）必须在被依赖者
  已发布之后立刻做；`extensionsInitialized()` 只适合**供给侧**收尾（在确保自己就绪之后再对外发布）。
- **`initialize()` 必须幂等**：插件库一旦加载即常驻进程，**同一进程里第二个管理器会拿到同一个实例并再次
  调用 `initialize()`**。连接、注册这类副作用要能重复执行——典型做法是把连接存成成员，进来先 `disconnect`
  再重建。`src/modules/filetree/filetreeplugin.cpp` 顶部就是这个修法的注释。
- 一个插件 `initialize()` 返回 `false`，它**以及所有依赖它的插件**都不进本次运行，宿主照常启动。

## 三种依赖

| 类型 | 元数据 | 另一头在时 | 另一头不在 / 被关 / 失败 | 能否链接并直接调用对方 |
| :--- | :--- | :--- | :--- | :--- |
| **required** | 无 `Type`（或 `"required"`） | 先加载、先初始化 | **本插件也不启动**（连带） | 可以 |
| **optional** | `"Type":"optional"` | **同样先加载、先初始化** | **静默丢弃**，本插件照常启动 | 不可以 |
| **test** | `"Type":"test"` | 不参与排序 | 无影响 | — |

required 与 optional 的差别**只在"另一头不在时"**：能解析时两种依赖都会让对方**先**加载、先初始化
（这样本插件才来得及查到对方已发布的东西）；区别在于 required 缺席会连带关掉本插件，optional 缺席则
**当没声明过**。

> ⚠️ **`PLUGIN_RECOMMENDS` 不是 Qt Creator 的 `Recommends`**。我们的 `PLUGIN_RECOMMENDS` 生成的是
> `Dependencies` 里 `"Type" : "optional"` 的那一项（对应 Qt Creator 的 `PluginDependency::Type::Optional`）；
> 而 Qt Creator 另有一个**同名不同义**的 JSON 键 `Recommends`，含义是"启用本插件时顺带**启用**列出的插件"，
> 与加载顺序无关。两者的相似只在名字。

`src/modules/filetree` 是可选依赖的样板：它把打开的文件报给输出面板，但没有输出面板也能用——所以写
`PLUGIN_RECOMMENDS output` 而不是 `PLUGIN_DEPENDS output`。于是"关掉 output 连带关掉文件树"这个副作用
不存在了，而元数据里仍然声明了这层关系，输出面板在时它依然**先**被初始化。

## 插件与宿主：QxPluginContext

@ref QxPlugin::QxPluginContext 是插件**唯一**的宿主句柄——插件看不到宿主窗口类，宿主类名因此波及不到
任何插件（K14 据此定案为**保留 `QxAppShell`**，见 [K14](https://github.com/canpool/qtcanpool/blob/master/doc/design/4.0-TASKS.md)）。
它暴露宿主允许插件使用的全部能力：

| 能力 | 方法 |
| :--- | :--- |
| 页面 | `addPage` / `setCurrentPage` / `currentPageId` |
| 停靠面板 | `addDock(area, id, title, widget)` |
| 状态栏 | `setStatusMessage` / `setBusy` / `setProgressRange` / `setProgress` / `clearProgress` |
| 通知 | `showToast(text, level, timeoutMs)` |
| 配置 | `settings()`（**永不为 null**） |
| 对象池 | `addObject` / `removeObject` / `objects` / `objectByName` / `object<T>()` |

要点：

- **加能力 = 有意扩大契约**：往这个接口上添一条，就是插件契约的一次**版本化扩展**（`QX_PLUGIN_IID` 上的
  `/1.0` 与 `QxPlugin::interfaceVersion()` 是它的版本锚）。宿主特有的东西留在具体宿主类里，不进这个接口。
- **接口的契约自包含、宿主中立**：文档里不许出现"契约见 `QxAppShell::addPage`"这类把契约推给具体宿主的
  说法；它面向的是"宿主 / application shell"这个角色。
- **`settings()` 是插件依赖 `qxcore` 的唯一原因**：返回类型 `QxCore::QxSettings *` 出现在**公开签名**里，
  所以 `qxplugin → qxcore` 是 PUBLIC 依赖。归属与是否切掉的讨论见
  [`4.0-TASKS.md` K19](https://github.com/canpool/qtcanpool/blob/master/doc/design/4.0-TASKS.md)。

## 两个插件相遇：对象池

一个插件没有通向同伴的句柄，所以"我有话要说"和"我有地方放"这两端自己找不到对方。
@ref QxPlugin::QxObjectPool 就是它们相遇的地方：

```cpp
// provider，在 initialize() 里：
setObjectName(QStringLiteral("output"));
context->addObject(this);

// consumer，在 initialize() 里：
QObject *sink = context->objectByName(QStringLiteral("output"));
if (sink) {
    m_sinkConnection = connect(this, SIGNAL(fileActivated(QString)), sink, SLOT(appendLine(QString)));
    if (!m_sinkConnection)
        qWarning("filetree: the pooled 'output' offers no appendLine(QString)");
}
```

要点：

- **发布是主动的**：不发布就不可达；池只装别人选择放进去的东西。
- **按名 vs 按类型**：两端**共享声明**（同一宿主服务，或双方都链接同一个库里的类型）时用 `object<T>()`；
  **不共享任何头文件**时用 `objectByName()`，拿回的东西经**元对象**驱动（`SIGNAL/SLOT` 字符串连接）。
  样板里的两个模块走的是后者。
- **不 include 对方的头，也不链接对方**：这正是让两端可以各自缺席的原因。代价是字符串连接**没有编译期
  检查**——所以连接失败必须显式 `qWarning`，别让它悄悄什么都不做。
- **池不持有所有权**：内部是 `QPointer`，对象销毁自动出池，不会留下悬垂指针。发布方在 `shutdown()` 里
  `removeObject(this)` 是更整齐的习惯——在自己消失**之前**先退场，而不是被池发现指针空了。
- **为什么不 include 对方的头直接调 `appendLine()`？**——那会把两个库焊成一件事（互相 include 等于互相
  依赖），一端的缺席就**编译不出来**；而"两端可以各自不在"正是可选依赖与池要买的东西。宿主侧同理：宁可让
  两个插件在池里自接线，也不在宿主里写一张 `(源 id, 信号, 目标 id, 槽)` 的表——那会让宿主认识它本不该
  认识的模块名，而"宿主不认识任何模块"是整个体系的前提。

## 启用 / 停用与失败隔离

- **停用列表 / 启用列表**：@ref QxPlugin::QxPluginManager 的 `setDisabledPlugins()` 与 `setEnabledPlugins()`。
  它们记的是**相对元数据的偏离**——与 `EnabledByDefault` 一致的插件两个列表都不进，所以"把默认开着的关掉"
  和"把默认关掉的打开"都能跨重启存活。两者冲突时**停用列表赢**。开关在**下次启动**时生效：管理器只在
  `loadPlugins()` 之前读一次。
- **@ref QxApp::QxPluginManagerDialog**：一个现成的管理器界面（列表 / 加载序 / 版本 / 状态 / 依赖 /
  诊断 / 开关）。它**从不加载或卸载任何东西**，只写上面两个列表；四个静态助手负责读写：

```cpp
// 启动时，loadPlugins() 之前
manager.setDisabledPlugins(QxPluginManagerDialog::disabledPlugins(settings));
manager.setEnabledPlugins(QxPluginManagerDialog::enabledPlugins(settings));
manager.loadPlugins();

// 之后，从某个菜单项打开
QxPluginManagerDialog dialog(&manager, settings, &shell);
dialog.exec();
```

- **失败隔离**：任何单个插件加载失败、或在 `initialize()` 返回 `false`，都只把它（及其依赖链）**搁置**，
  宿主继续启动。查询用 `hasError()` 与 `errorString()`（每条错误带插件 id 前缀）；管理器界面下方的诊断区
  就是 `errorString()` 的展示位——环、级联这类牵涉多个插件的错误在那里**整段**出现。
- **`src/modules/broken`** 是一个**故意跑不起来**的样例（依赖一个不存在的插件），刻意不进构建——它留作
  文档与截图用；把它放进插件目录就能看到"缺依赖"在界面上长什么样。

## 静态构建（WASM）下的插件

`QTC_STATIC_BUILD=ON`（`wasm` 预设即如此）时 `add_qtc_plugin()` 产出的是**静态库**，
**没有运行时插件发现**：静态插件只有被应用显式 `Q_IMPORT_PLUGIN` 才进得来，而那是"应用认识每一个模块"，
与插件体系的前提正面冲突。因此：

- 静态构建里用 @ref QxPlugin::QxPluginManager 的 `registerStaticPlugin()` 在**编译期**登记插件——
  解析、排序、失败隔离的代码与动态路径**完全相同**；
- WASM demo 演示的是**框架本身**（外壳 / 主题 / 停靠），**不演示**插件发现；
- `IdeShellDemo` 在静态配置下**不构建**（它靠扫描插件目录工作）。

判据与实测见 [`4.0-TASKS.md` 的 D1 / K17](https://github.com/canpool/qtcanpool/blob/master/doc/design/4.0-TASKS.md)。

## 在仓库外构建插件

一个只依赖 `QtCanpool::qxplugin` 的外部 CMake 工程即可产出插件：`add_qtc_plugin()` 随安装的
`QtCanpool` 包一起导出，用法与仓库内的模块完全一致。外部工程的**骨架**见仓库里的
[`projects/plugin`](https://github.com/canpool/qtcanpool/tree/master/projects/plugin)——它同时给出
插件与一个最小宿主，CI 会真跑一遍；`projects/consume` 与 `projects/template` 则是**应用**骨架，
把 `add_executable` 换成下面的 `add_qtc_plugin` 即得到插件工程：

```cmake
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)
find_package(QtCanpool REQUIRED)

add_qtc_plugin(myplugin
  PLUGIN_CLASS MyPlugin
  DEPENDS QtCanpool::qxplugin Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
  SOURCES myplugin.cpp myplugin.h
)
```

把生成的库放进宿主扫描的插件目录（`RELATIVE_PLUGIN_PATH` 指向的位置）即可被 @ref QxPlugin::QxPluginManager
认出，**不写一句注册代码**。

> ⚠️ `RELATIVE_PLUGIN_PATH` 是**相对 `IDE_BIN_PATH`**（Linux 上就是 `bin/`）算的，所以它只对
> **放在那里的可执行文件**成立。而 `add_qtc_executable()` 默认落到 `IDE_LIBEXEC_PATH`
> （Linux 是 `libexec/qtproject/`）——两者不是一回事，用默认值算插件目录会**差一层**，
> 且宿主照样启动、只是插件全都没被发现。真要按 `RELATIVE_PLUGIN_PATH` 找插件，就给宿主显式
> 写 `DESTINATION "${IDE_BIN_PATH}"`（`IdeShellDemo` 就是这么做的）。

## 参考

- 框架源码：`src/libs/qxplugin/`（`QxPlugin` / `QxPluginContext` / `QxPluginSpec` / `QxPluginManager` /
  `QxObjectPool`）
- 管理器界面：@ref QxApp::QxPluginManagerDialog（`src/libs/qxapp/qxpluginmanagerdialog.h`）
- 样板模块：`src/modules/{output,notebook,filetree}`（外加 `broken/` 失败样例）
- 样板应用：`demos/qxapp/ideshell`——"不认识任何模块"的宿主
- 测试：`tests/qxplugin`、`tests/qxmodules`、`tests/ideshell`
- 设计与决策：[`4.0-TASKS.md`](https://github.com/canpool/qtcanpool/blob/master/doc/design/4.0-TASKS.md)
  （B1~B5、K15~K19）
