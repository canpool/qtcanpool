# ffmpeg库
- 官网：https://ffmpeg.org/download.html#build-windows
- 版本：6.1-full_build-www.gyan.dev
- 架构：x86_64
- 压缩包：ffmpeg-6.1-full_build-shared.7z
- 下载地址：https://www.gyan.dev/ffmpeg/builds/#release-builds

# 使用
由于 ffmpeg 库较大，只提供连接库文件 ffmpeg.pri，如果要使用 ffmpeg，只需下载上述压缩包，然后解压到 qtcanpool 的同级目录 3rdparty 目录下的 ffmpeg 目录中。

目录结构示例如下：
```
3rdparty
    ffmpeg
        ffmpeg-6.1-full_build-shared
qtcanpool
    thirdparty
        ffmpeg
            ffmpeg.pri
```