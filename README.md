
### 简介
本项目是根据 QtCreator 源码结构总结出的一套通用的项目管理模板。核心库为基于 QtWidgets 的 qcanpool，同时，集成常用组件和第三方类库。

qtcanpool 旨在提供给用户优秀的项目管理方式、多样的选择、优质的控件……

### 仓库
- github：[https://github.com/canpool/qtcanpool](https://github.com/canpool/qtcanpool)
- gitee：[https://gitee.com/icanpool/qtcanpool](https://gitee.com/icanpool/qtcanpool)

### 教程
- [使用教程](https://blog.csdn.net/canpool/category_10631139.html)
- [初学教程](https://blog.csdn.net/liang19890820/article/details/50277095)
- [学习之路](https://www.devbean.net/category/qt-study-road-2/)

### 目录
|一级目录|二级目录|说明|
|:------|:------|:------|
|demos||丰富的例子|
|doc||文档|
|examples||控件类的示例
|include||公共头文件
|others||子模块的示例
|projects||项目目录，其中提供简单的模板 template，用户可以持续添加自己的项目在此目录，从而实现一套框架管理多项目
|scripts||辅助的脚本文件
|src||源代码文件
||libs|基础类库
||modules|基础模块，实用的代码，但不具备类库的体量
||plugins|基础插件
||shared|共享的实用代码文件
|tests||测试用例

### 环境
- Qt 6.5.1 MinGW/MSVC2019 64bit
- Qt 5.15.2 MinGW/MSVC2019 64bit
- Qt 5.14.2 MinGW 64bit
- 其它环境未测试，推荐使用 [QT LTS](https://download.qt.io/official_releases/qt/) 版本

### 版本
- 格式：x.y.z
- 说明：主版本.次版本.补丁版本

### 分支
- [master](https://gitee.com/icanpool/qtcanpool/tree/master/)：V2 版本的主分支
- [v1_master](https://gitee.com/icanpool/qtcanpool/tree/v1_master/)：V1 版本的主分支

说明：
- v1_master 分支已停止维护，只修复严重 bug

### 协议
* 遵循 [MulanPSL2](./LICENSE) 开源许可协议
* 集成组件遵循各自开源许可协议

### 规范
* [Google C++ Style Guide](http://google.github.io/styleguide/cppguide.html)
* [Qt 编程风格与规范](https://blog.csdn.net/qq_35488967/article/details/70055490)
* 源文件全英文的采用 UTF-8 编码，包含中文的采用 UTF-8 with BOM 编码
* 代码 git 提交格式：[git 知：提交格式](https://blog.csdn.net/canpool/article/details/126005367)

### 贡献
* 欢迎提交 issue 对关心的问题发起讨论
* 欢迎 Fork 仓库，pull request 贡献
* 贡献者可在文件头版权中添加个人信息，格式如下：
```
/**
 * Copyright (C) YYYY NAME <EMAIL>
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
```

### 交流
* QQ群：831617934（Qt业余交流）

### 例子
- <font size=4>fancydemo</font>

![qcanpool](./doc/pics/fancydemo.png)

- <font size=4>fancytitlebar</font>

![fancytitlebar](./doc/pics/fancytitlebar.png)

支持 windows 原生特性（毛玻璃预览，半屏和¼屏等显示效果），仅支持单屏

- <font size=4>fancyribbon</font>

![fancyribbon](./doc/pics/fancyribbon.png)

- <font size=4>fancytoolbutton</font>

![fancyribbon](./doc/pics/fancytoolbutton.png)

- <font size=4>qrcodedemo</font>

![qrcodedemo](./doc/pics/qrcodedemo.png#pic_center)

- <font size=4>ribbondemo</font>

![ribbondemo](./doc/pics/ribbondemo.gif#pic_center)

### 体验
下载源码，使用 qtcreator 打开 qtcanpool.pro，然后右击 fancydemo 并单击 Run 体验，如下所示：
![run](./doc/pics/run.png)

### 其它
- QxRibbon：[https://gitee.com/icanpool/QxRibbon](https://gitee.com/icanpool/QxRibbon)：Microsoft Ribbon for Qt


### 案例

虚位以待中……

### 后语

欢迎广大 Qt 开源爱好者加入其中，乐在其中……

### 赞助

如果您觉得本项目对您有很大帮助，甚至从中获取了一定的商业回报，您可以赞助该项目，助其更好的发展。

赞助方式：

![sponsor](./doc/sponsor/sponsor.png)

赞助名单：[名单](./sponsor.md)
