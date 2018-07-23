## Qt编译和使用boost库
[参考](https://blog.csdn.net/guoqianqian5812/article/details/52654914)

## 环境
* Qt 5.10.0 MinGW 32bit
* [boost_1_67_0](https://www.boost.org/)

## 一、配置gcc环境
### 1.1 添加MinGW到环境变量
安装Qt 5.10.0 MinGW 32bit默认已经将MinGW的环境配置到windows的环境变量中了，如果您安装的版本没有，可以自行添加，我的在D:\Qt\Qt5.10.0\Tools\mingw530_32\bin

### 1.2 验证环境配置
打开windows的cmd.exe (Win+R,输入cmd回车)，输入以下内容

```
gcc -v
```
如果显示了gcc的版本信息，证明MinGW环境添加成功。

## 二、编译boost库
### 2.1 生成b2.exe和bjam.exe
打开cmd，进入boost库所在目录，找到build.bat，然后执行以下命令编译b2和bjam

```
build gcc
```
我的build.bat在D:\products\qtcanpool\3rdparty\boost_1_67_0\tools\build\src\engine，
编译完后在当前目录会生成一个bin.ntx86的目录，进入后有b2.exe和bjam.exe可执行文件，将这两个文件拷贝到boost源代码的根目录下（如：D:\products\qtcanpool\3rdparty\boost_1_67_0）

### 2.2 执行安装

```
bjam --toolset=gcc --prefix=./bin install
```
时间比较长，编译好后，我的在C:\Boost目录下生成include和lib两个文件夹，include里面是boost的头文件（编译时从boost源码中拷贝而来），lib里面是生成的静态库（release和debug版都有）

## 三、使用boost库
由于boost库文件太大，就不放在qtcanpool/thirdparty/boost中，默认就放在C:\Boost，boost.pri用来连接boost库，boostdemo是用例。