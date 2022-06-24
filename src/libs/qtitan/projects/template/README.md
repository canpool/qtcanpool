<!-- TOC -->

- [1. 简介](#1-简介)
    - [1.1. 目录简介](#11-目录简介)
    - [1.2. 文件简介](#12-文件简介)
        - [1.2.1. qtitanconfig.pri](#121-qtitanconfigpri)
        - [1.2.2. template.pro](#122-templatepro)
        - [1.2.3. qtproject.pri](#123-qtprojectpri)
- [2. 使用](#2-使用)

<!-- /TOC -->

# 1. 简介
本工程模板的目的是方便开发者基于Qtitan系列控件进行开发，模板基本和Qtitan控件解耦，具备存放灵活、操作简单、修改范围小等特点。

## 1.1. 目录简介
|一级目录|二级目录|说明|
|:------|:------|:------|
|doc||文档相关|
|qtitanribbon|qtitanribbon.pro|链接QtitanRibbon源码|
|test||存放用户自己的开发的代码|
|qtitanconfig.pri||Qtitan的配置文件|
|template.pro||工程文件|

备注：
1. 链接源码的设计是为了实现库文件(lib/dll)和应用文件(exe)能输出到相同目标目录；
2. Qtitan的配置文件的设计是为了实现修改最小化。

## 1.2. 文件简介
### 1.2.1. qtitanconfig.pri
```qt
!isEmpty(QTNCONFIG_PRI_INCLUDED):warning("qtitanconfig.pri already included")
QTNCONFIG_PRI_INCLUDED = 1

QTITANDIR = $$quote($$(QTITANDIR))
isEmpty(QTITANDIR):QTITANDIR = $$quote($$PWD/../../)
```
主要配置QTITAN的目录变量：QTITANDIR

### 1.2.2. template.pro
```qt
TEMPLATE = subdirs

CONFIG += ordered

include(qtitanconfig.pri)

SUBDIRS =           \
    qtitanribbon    \
    test
```

1. SUBDIRS说明

   添加qtitanribbon时，编译时将一同编译Qtitan的相关源码。开发代码过程中，第一次编译Qtitan源码后会生成lib和dll库，之后可以注释掉。

2. CONFIG += ordered说明

   添加这个选项，编译时将按顺序编译，SUBDIRS中目录顺序要根据依赖调整。可以选择不加这个选项。


### 1.2.3. qtproject.pri
这个文件来自qtcanpool，进行了简化。

# 2. 使用

通过上述的简介，template工程中跟Qtitan强相关的一点就是qtitanconfig.pri中的QTITANDIR变量，所以template工程原则上可以随意
