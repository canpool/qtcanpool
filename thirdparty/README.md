# 说明
本目录主要提供一些链接第三方库进行开发的方法

# 形式
通常，第三方库是以 include 和 lib 等非源码的形式提供

# 位置
默认，第三方库放置在与 qtcanpool 同级的目录 3rdparty 中，形如：
```
3rdparty
    Boost
    ...
qtcanpool
    ...
    thirdparty
        boost
        ...
```
可以根据实际情况放置，同时修改相关库的 pri 文件中的路径即可
