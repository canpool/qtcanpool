<!-- TOC -->

- [1. 概述](#1-概述)
- [2. 版本](#2-版本)
- [3. 环境](#3-环境)
- [4. 编译](#4-编译)
    - [4.1. 构建编译引擎](#41-构建编译引擎)
    - [4.2. 构建boost库](#42-构建boost库)
- [5. 使用](#5-使用)
- [6. 参考](#6-参考)

<!-- /TOC -->

# 1. 概述
本文简单介绍如何通过 Qt 编译和使用 boost 库。

# 2. 版本
* Qt 5.15.2 (MinGW 8.1.0 64-bit)
* [boost_1_83_0](https://www.boost.org/)

# 3. 环境
打开 Qt 安装时带的 windows 命令行终端工具，如：Qt 5.15.2 (MinGW 8.1.0 64-bit) ，输入以下内容：
```
gcc -v
```
如果显示了 gcc 的版本信息，证明 MinGW 环境可用。

# 4. 编译
## 4.1. 构建编译引擎
在命令行终端中，进入 boost 源码目录，然后执行以下命令构建编译引擎 b2.exe
```
bootstrap.bat gcc
```
构建完成后，会在 boost 源码目录下生成 b2.exe 工具，同时会提示 b2.exe 的用法。

## 4.2. 构建boost库
接着，在命令行终端中，执行如下命令构建 boost 库：
```
.\b2 toolset=gcc --prefix=.\Boost install
```
时间比较长，编译好后，会在源码目录内的 Boost 目录下生成 include 和 lib 两个文件夹。
- include 里面是 boost 的头文件（编译时从 boost 源码中拷贝而来）
- lib 里面是生成的静态库（release 和 debug 版都有）

# 5. 使用
由于 boost 库文件太大，就不放在 qtcanpool/thirdparty/boost 中，默认将上面生成的 Boost 目录拷贝到 qtcanpool 的同级目录 3rdparty 目录中。

目录树大致如下：
```
├─qtcanpool
│  ├─thirdparty
│     └─boost
│  │      └─boost.pri
├─3rdparty
│  ├─Boost
│     │─include
│     └─lib
```

boost.pri 用来连接 boost 库，boostdemo 是用例。

# 6. 参考
- [Qt编译和使用boost库](https://blog.csdn.net/guoqianqian5812/article/details/52654914)
- [Qt Creator5.12配置boost库经验分享](https://blog.csdn.net/King_zkk/article/details/105814959)
