# 组件全览

本页按库列出能力范围、主要类与最小用法。类与函数的完整签名见各库对应的命名空间页面：

- qxcore → @ref QxCore
- qxtheme → @ref QxTheme
- qxwindow → @ref QxWindow
- qxribbon → @ref QxRibbon
- qxdock → @ref QxDock
- qxplugin → @ref QxPlugin
- qxapp → @ref QxApp

> `qcanpool` 已于 **3.2 删除**（含 3.1 留下的转发头），本页不再列出。迁移见
> [迁移指南](migration.md)。

## qxcore — 基础设施

只依赖 Qt Core，任何库都可以使用。

| 类 | 说明 |
| :--- | :--- |
| @ref QxCore::QxSettings | `QSettings` 的类型化包装：带默认值的读取、分组、以及带版本号的配置迁移 |
| @ref QxCore::QxLogger | 基于 Qt 消息处理器的日志：级别过滤、时间戳、按大小轮转的文件输出 |
| @ref QxCore::QxTranslator | 翻译装载与切换：扫描目录里的 `.qm`、安装语言包、发出切换信号（3.2） |

```cpp
#include "qxcore/qxlogger.h"
#include "qxcore/qxsettings.h"
#include "qxcore/qxtranslator.h"

using namespace QxCore;

QCoreApplication::setOrganizationName(QStringLiteral("acme"));
QCoreApplication::setApplicationName(QStringLiteral("MyApp"));

QxLogger::Options options;
options.fileName = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                   + QStringLiteral("/myapp.log");
options.level = QxLogger::Info;
QxLogger::init(options);

QxSettings settings(QStringLiteral("acme"), QStringLiteral("MyApp"));
settings.beginGroup(QStringLiteral("ui"));
const int width = settings.intValue(QStringLiteral("width"), 1280);
settings.setValue(QStringLiteral("width"), width + 100);
settings.endGroup();
```

`QxSettings` 的迁移接口用于跨版本调整配置结构：注册若干 `(版本号, 回调)`，
调用 `migrate(targetVersion)` 时会**按版本升序**执行中间缺失的每一步，
已到达或超过目标版本时返回 0、不做任何事（因此不会"降级"）。

`QxLogger::init()` 会接管进程级消息处理器，并在 `shutdown()` 时恢复此前的处理器，
所以它与调用方自行安装的处理器可以共存，顺序为「先装者被后装者包裹」。

### i18n 语言切换（3.2）

`QxTranslator` 只补 Qt 缺的**记账**部分：Qt 自己已经会为装载翻译后的顶层部件投递
`QEvent::LanguageChange`，缺的是「有哪些语言」「哪些文件真的装上了」以及「怎么通知那些
**不是部件**的使用者」。模型与控制器收不到 `QEvent::LanguageChange`，`languageChanged()`
就是为它们准备的。

```cpp
#include "qxcore/qxsettings.h"
#include "qxcore/qxtranslator.h"

using namespace QxCore;

QxSettings settings;
QxTranslator translator;
translator.setTranslationPath(QCoreApplication::applicationDirPath());

// 选中的语言是普通配置：读出来装上，变了再写回去
QObject::connect(&translator, &QxTranslator::languageChanged, &settings, [&](const QString &language) {
    settings.setValue(QStringLiteral("ui/language"), language);
});
translator.setLanguage(settings.stringValue(QStringLiteral("ui/language"), QLocale::system().name()));
```

要点：

- **语言来自目录**：`availableLanguages()` 读的是 `<前缀>_<语言>.qm` 的文件名，
  前缀由文件自己带，所以应用自己的语言包与放在旁边的 Qt 语言包会被一起枚举。
- **应用的语言包必须装上一个**：装载失败时什么都不变，也就是**装不上的语言不会让应用
  掉进空界面**；`qtbase_<语言>.qm` 只是补充，缺了不算失败、有了也不算成功。
- **持久化留给调用方**：语言只是一个值，而应用本来就有一份 `QxSettings`——上面那三行
  比在这里多一个依赖更划算。

## qxtheme — 主题引擎

把「调色板」与「样式表」作为一件事统一应用。详见[主题引擎](theming.md)。

| 类 / 函数 | 说明 |
| :--- | :--- |
| @ref QxTheme::QxThemeManager | 进程级单例：应用主题、持久化选择、跟随系统深浅色 |
| @ref QxTheme::Theme | 内置主题枚举 |
| @ref QxTheme::themeId / themeName / themeFromId | 稳定 id、显示名与相互转换 |
| @ref QxTheme::isDarkTheme | 判断主题是否为深色 |
| @ref QxTheme::themeStyleSheetPath / themeStyleSheet | 内置样式表的资源路径与内容 |

```cpp
#include "qxtheme/qxthememanager.h"

using namespace QxTheme;

QxThemeManager *themes = QxThemeManager::instance();
themes->setTheme(DarkOfficePlus);
```

## qxwindow — 自定义窗口

无边框窗口与系统按钮代理，是 Ribbon 界面的底座。

| 类 | 说明 |
| :--- | :--- |
| `WindowAgent` / `WindowAgentBase` | 无边框窗口代理：拖动、缩放、系统按钮命中测试 |
| `WindowButtonGroup` | 一组最小化 / 最大化 / 关闭按钮 |
| `WindowContext` | 原生窗口上下文（Windows / Linux / macOS 各一份实现） |

`QWK` 时代的名字已全部并入 `QxWindow` 命名空间。

## qxribbon — Ribbon 界面

| 类 | 说明 |
| :--- | :--- |
| `RibbonBar` | Ribbon 主体：应用按钮、快捷工具栏、页与分组 |
| `RibbonPage` / `RibbonGroup` | 页与分组，分组内可放入大按钮、小按钮、下拉菜单 |
| `RibbonMainWindow` | 已装配好 RibbonBar 的主窗口 |
| `RibbonTheme` | widget 级样式表加载器（6 套内置样式表）。新代码请优先使用 @ref QxTheme::QxThemeManager |
| `RibbonGallery` / `RibbonCustomizeDialog` 等 | 选项板、自定义对话框等配套控件 |

样式表资源位于 `:/qxribbon/res/stylesheets/*.css`，是**进程级**资源；
`QxThemeManager` 正是按路径从这里取内置主题。

## qxdock — 可停靠窗口

| 类 | 说明 |
| :--- | :--- |
| `DockWindow` | 停靠区管理：`DockWidgetArea`、中央部件、状态保存与恢复 |
| `DockWidget` | 单个停靠面板（可移动、可浮动、可关闭） |
| `DockPanel` / `DockContainerWidget` / `DockAreaWidget` | 面板、容器与区域实现 |
| `DockFloatingWindow` | 浮动窗口 |

`DockWindow` 要求**中央部件第一个加入**：@ref QxApp::QxAppShell 在构造时就把页面栈装为中央部件，
其余停靠面板再围绕它排布。

## qxplugin — 插件体系

把「应用」与「它由哪些模块组成」分开：宿主只认识框架，模块以插件形式在运行时被发现、排序、加载。
完整的开发指南（元数据逐字段、三种依赖、生命周期、对象池写法、失败隔离）见[插件体系](plugins.md)。

| 类 | 说明 |
| :--- | :--- |
| @ref QxPlugin::QxPlugin | 插件基类：`initialize` / `extensionsInitialized` / `shutdown` 三个生命周期回调 |
| @ref QxPlugin::QxPluginContext | 插件面向的宿主契约：页面 / 停靠 / 状态栏 / 通知 / 配置 / 对象池；插件看不到宿主窗口类 |
| @ref QxPlugin::QxPluginSpec | 单个插件的元数据与运行时状态（只读视图） |
| @ref QxPlugin::QxPluginManager | 发现 / 拓扑排序 / 加载 / 失败隔离；动态走 `QPluginLoader`，静态走 `registerStaticPlugin()` |
| @ref QxPlugin::QxObjectPool | 插件间软协作：发布、按名或按类型查找；只认 `QObject`，两端无需互相 include |

```cpp
#include "qxplugin/qxplugin.h"
#include "qxplugin/qxpluginmanager.h"

using namespace QxPlugin;

// 宿主侧：把外壳的上下文交给管理器，再加载
QxPluginManager manager;
manager.setContext(shell.pluginContext());
manager.setPluginPaths({shell.pluginDirectory()});
manager.loadPlugins();
```

要点：

- **两级依赖**：`PLUGIN_DEPENDS`（硬，缺席则本插件也不启动）与 `PLUGIN_RECOMMENDS`（软，缺席则静默跳过）。
  只有硬依赖允许直接链接并调用对方。
- **失败隔离**：单个插件失败只搁置它及其依赖链，宿主照常启动；用 `hasError()` / `errorString()` 查询。
- **对象池**：让两个不共享头文件、不互相链接的插件协作——一方向池发布，另一方按名查找并经元对象连接。
- **静态构建**（WASM）**没有运行时发现**，用 `registerStaticPlugin()` 在编译期登记。

## qxapp — 应用框架

| 类 | 说明 |
| :--- | :--- |
| `RibbonAppWindow` | 无边框 Ribbon 主窗口（RibbonBar + 窗口代理） |
| @ref QxApp::QxAppShell | 应用外壳：导航轨 + 页面栈 + 停靠区 + 状态栏 + 布局持久化。详见[应用外壳](appshell.md) |
| @ref QxApp::QxNavigationBar | 竖排互斥导航轨 |
| @ref QxApp::QxSplashScreen | 带进度与消息的启动屏 |
| `QxToolButton` / `QxExtensionButton` / `QxMenuButton` / `QxMenuAccessButton` | 工具按钮与菜单按钮（3.1 自 `qcanpool` 迁入） |
| `QxTabBar` / `QxTabWidget` / `QxNavBar` | 轻量 Tab 与导航（3.1 自 `qcanpool` 迁入） |
| @ref QxApp::QxToast | 应用内提示条：带级别、自动消失、悬停暂停（3.2） |
| @ref QxApp::QxToastManager | 同一宿主窗口的 toast 堆叠、超时与淘汰（3.2） |
| @ref QxApp::QxProperty | 一个设置项的描述：键、类型、默认值与可选项（3.2） |
| @ref QxApp::QxPropertyEditor | 由 `QxProperty` 列表生成的键/值表单（3.2） |
| @ref QxApp::QxSettingsDialog | 设置对话框：左页列表 + 右表单，负责读盘与写盘（3.2） |
| @ref QxApp::QxWorkspaceManager | 命名工作区：存 / 取 / 改名 / 删除，切换时只动停靠布局与当前页（3.3） |
| @ref QxApp::QxPluginManagerDialog | 插件管理器界面：列表 / 加载序 / 状态 / 依赖 / 诊断 / 启停开关（4.0） |

### Toast 应用内通知（3.2）

`QxToast` 是贴在宿主窗口上的一条短提示，`QxToastManager` 管它的堆叠与淘汰；
`QxAppShell` 自带一个 manager，`showToast()` 开箱可用。

```cpp
#include "qxapp/qxtoast.h"
#include "qxapp/qxtoastmanager.h"

using namespace QxApp;

// 对外壳说一句就够了
shell.showToast(tr("文档已保存"), QxToast::Success);

// 或者自己管一个栈：位置、同屏上限、超时
QxToastManager *toasts = new QxToastManager(&shell);
toasts->setPosition(QxToastManager::BottomRight);
toasts->setMaxVisible(3);
toasts->show(tr("设备没有回应"), QxToast::Error, 10000);
```

要点：

- **不依赖窗口管理器**：toast 是宿主窗口的**子控件**，不是 `Qt::ToolTip` 顶层窗口——
  因此不置顶、不抢焦点，且在 offscreen 与 WebAssembly 下行为一致。
- **级别 → 配色走调色板**：底板取 `QPalette::ToolTipBase`、文字取 `QPalette::ToolTipText`、
  强调色取 `QPalette::Highlight` 的饱和度与亮度而只挪动色相（绿/琥珀/红），
  不写死任何颜色，随主题联动。
- **超时与悬停**：每条一个时钟；悬停时暂停计时，移开后接着走剩下的时间，
  而不是重新计一遍。
- **同屏上限**：超过 `maxVisible()` 时最旧的一条被淘汰，一批消息留下最后几条。
- **队列与动画分离**：`show()` 立即入栈、每次淘汰立即出栈，动画在记账之后跑，
  所以 `count()` / `toasts()` 报的就是屏幕上真实的状态。

### 属性编辑器内核（3.2）

设置界面由**描述**生成，而不是手写控件。`QxProperty` 是一个设置项的声明——键、类型、
默认值——`QxPropertyEditor` 把一组声明变成一张表单：

```cpp
#include "qxapp/qxproperty.h"
#include "qxapp/qxpropertyeditor.h"

using namespace QxApp;

QxProperty language;
language.key = QStringLiteral("ui/language");
language.label = tr("语言");
language.type = QxProperty::Enum;
language.choices = QStringList{QStringLiteral("zh_CN"), QStringLiteral("en")};
language.defaultValue = QStringLiteral("zh_CN");

QxPropertyEditor *editor = new QxPropertyEditor;
editor->addGroup(tr("界面"), QList<QxProperty>{language});

// 整批写入，也整批读出：与配置文件的一次往返
QHash<QString, QVariant> stored;
stored.insert(QStringLiteral("ui/language"), QStringLiteral("en"));
editor->setAllValues(stored);

connect(editor, &QxPropertyEditor::valueChanged, [](const QString &key, const QVariant &value) {
    // key == "ui/language"
});
```

要点：

- **类型归一**：写进模型的值一定先转成该项声明的类型，所以从配置文件里读到的
  `"80"` 与微调框给出的 `80` 是同一个值；转不了的会被拒绝（并 `qWarning`），
  而不是悄悄变成 0。
- **两个「回到原样」**：`reset()` 回到声明的默认值（重装的样子），
  `resetToInitial()` 回到 `setAllValues()` 传进来的那批值（放弃我刚才的编辑）。
- **枚举不丢未知值**：存档里出现列表中没有的选项时，它被追加进列表而不是被丢掉，
  于是「这一项为什么空了」不会发生。
- **九种类型**：`Bool` / `Int` / `Double` / `String` / `Text` / `Enum` / `Color` /
  `Font` / `Path`，各自对应一种控件；`Color` 与 `Font` 的对话框**只在点击时**才弹。

### 设置对话框（3.2）

`QxPropertyEditor` 管值、不知道值从哪来；`QxSettingsDialog` 是知道**存储**的那一半——
把值读出来，再把值写回去。

```cpp
#include "qxapp/qxsettingsdialog.h"

using namespace QxApp;

QxSettingsDialog dialog(shell.settings());
dialog.addPage(QStringLiteral("general"), generalIcon, tr("常规"), generalProperties);
dialog.addPage(QStringLiteral("editor"), editorIcon, tr("编辑器"), editorProperties);
dialog.addGroup(QStringLiteral("editor"), tr("缩进"), indentationProperties);
dialog.exec();
```

要点：

- **显示时才读**：读取发生在 `showEvent()`，不在构造时——后加的页也会被读到，
  而且第二次打开时看到的是**存档里的值**，不是上次编辑又被丢弃的那批。
- **三个动作**：`apply()` 写盘并 `sync()`；`accept()` = 应用 + 关闭；
  `reject()` = `resetToInitial()` + 关闭，所以「取消」在界面上也真的回到了原样。
- **Apply 跟着改动亮灭**：应用之后表单以屏幕上的值为新基线，按钮随即变灰，
  不需要重读一遍。
- **键即路径**：`QxProperty::key` 是存储路径，因此必须稳定、**不翻译**；
  页标题与项标签是展示，可以自由翻译。没写过的键读出来就是它的 `defaultValue`，
  这正是首次启动看起来像已配置的原因。
- **迁移不在这里**：升级存档布局是**启动期**的事——它要在任何东西读配置之前跑，
  还可能碰到对话框里根本没有的键——所以留给 `QxSettings::migrate()`。

### 命名工作区（3.3）

外壳本来就持久化一份布局，但那份**没有名字**。`QxWorkspaceManager` 补上名字：

```cpp
#include "qxapp/qxworkspacemanager.h"

using namespace QxApp;

QxWorkspaceManager *workspaces = shell.workspaceManager();
workspaces->saveWorkspace(tr("Writing"));    // 存下当前排列，并使其成为当前工作区
workspaces->applyWorkspace(tr("Writing"));   // 排列回来

for (const QString &name : shell.workspaceNames()) {   // 菜单按保存顺序枚举
    menu->addAction(name);
}
```

要点：

- **一个工作区 = 停靠布局 + 当前页**，就这两样。它**不存窗口几何**——
  几何属于窗口，切一套面板排列却把窗口搬走或改尺寸，在最大化与多显示器下是惊吓。
  几何仍归 `QxAppShell::saveLayout()` 管，那份「没有名字的工作区」每个应用免费得到一份。
- **不缓存**：名字、当前工作区、存着的布局每次都从 `QxSettings` 读回来，
  因此两个外壳指向同一个文件时口径一致——管理器是存储之上的一层薄壳，不是第二个事实来源。
- **保存即选中**：存下之后屏幕上的样子就是这个名字的含义，所以最后存的那份是当前工作区。
- **启动时不自动套用**：要不要套、套哪个由应用决定，框架只保证 `currentWorkspace()` 被记住。
- **改名发两个信号**（`workspaceRemoved(旧)` + `workspaceSaved(新)`），不另设 `renamed`，
  否则每个消费者都要为同一个结果多写一个分支。

详见 [应用外壳 · 工作区](appshell.md#autotoc_md*)，可运行的入口在 `demos/qxapp/appshell`
的 `Workspaces` 分组。

## qtcompat — 跨版本兼容

header-only 的辅助头，抹平 Qt 5 与 Qt 6 的少数接口差异，例如鼠标全局坐标与悬浮位置。

```cpp
#include "qtcompat/qtcompat.h"

const QPoint global = QtCanpoolCompat::globalMousePos();
```
