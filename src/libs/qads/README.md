# 添加 Qt-Advanced-Docking-System

Qt-Advanced-Docking-System 作为 qtcanpool 仓库的子模块，有如下机种方法下载子模块：

方法一：git clone --recursive

克隆 qtcanpool 仓库时，递归克隆所有子模块

```
​git clone --recursive https://gitee.com/icanpool/qtcanpool.git
```

方法二：git submodule update --init

克隆 qtcanpool 仓库时，没有克隆子模块，进入子模块目录进行子模块更新

```
​git clone https://gitee.com/icanpool/qtcanpool.git
cd qtcanpool/src/libs/qads/Qt-Advanced-Docking-System
git submodule update --init
```

方法三：下载 qtcanpool 压缩包

下载 qtcanpool 压缩包时，只能手动准备子模块代码，然后放到子模块目录中

# 使用 Qt-Advanced-Docking-System

Qt-Advanced-Docking-System 作为 qtcanpool 的一个库，如果应用程序需要使用该库，除了链接该库，还需要包含相关头文件。

由于该库作为 qtcanpool 仓库的子模块，qads 目录下没有头文件，所以不能通过

```
#include "qads/xxx.h"
```

的方式包含头文件。如果像下面这样包含又太长

```
#include "qads/Qt-Advanced-Docking-System/src/xxx.h"
```

所以，提供了 qads-inc.pri，内部设置了 INCLUDEPATH。应用程序的 pro 中如果定义了 QTC_LIB_DEPENDS += qads，在 qtproject.pri 中会自动包含 qads-inc.pri，否则需要在 por 中像下面这样包含该 pri

```
include($$QTCANPOOL_DIR/src/libs/qads/qads-inc.pri)
```

然后，应用程序中就可以直接包含相关头文件（不需要加 qads/ 前缀）

备注：显得有点繁琐，可以直接获取官方的 Qt-Advanced-Docking-System 想怎么用就怎么用。
