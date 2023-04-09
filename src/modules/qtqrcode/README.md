# 添加 qrencode
1. 下载 https://fukuchi.org/works/qrencode/ 代码，并解压进入源码目录
2. 将 config.h.in 改为 config.h，并在 config.h 中重新定义如下宏
    ```
    #define MAJOR_VERSION x
    #define MICRO_VERSION y
    #define MINOR_VERSION z
    #define VERSION "x.y.z"
    ```
3. 删除非 .h/.c 文件（可选）
4. 添加 qrencode.pri（定义宏 HAVE_CONFIG_H，并包含源码文件（除了qrenc.c））
