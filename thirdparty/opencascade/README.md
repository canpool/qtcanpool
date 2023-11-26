# opencascade库
- 官网: https://dev.opencascade.org/release
- 版本: 7.7.0 (07.11.2022)
- 安装文件: opencascade-7.7.0-vc14-64.exe (258 MB)
- 编译器: VC++ 2017 64 bit

# 第三方库
- 网址: https://dev.opencascade.org/resources/download/3rd-party-components

备注：官方安装包中已包含第三方库，此处第三方库主要是用于自编译 opencascade

# 环境准备

## Qt环境
- 版本：Qt 5.15.2 MSVC2019 64-bit

备注：由于官方安装包是基于 VC++ 2017 64 bit 构建的，所以选择的 MSCV 需要大于等于 2017，此处选择的 MSVC2019 (vc16) 兼容 2017 (vc14)。

## opencascade环境
下载官方安装文件，然后安装（安装位置无强制要求，作者安装的位置为 qtcanpool 的同级目录 3rdparty），最终目录结构如下所示：
```
3rdparty
    OpenCASCADE-7.7.0-vc14-64
        draco-1.4.1-vc14-64
        ffmpeg-3.3.4-64
        freeimage-3.17.0-vc14-64
        freetype-2.5.5-vc14-64
        opencascade-7.7.0
        openvr-1.14.15-64
        qt5.11.2-vc14-64
        rapidjson-1.1.0
        tbb_2021.5-vc14-64
        tcltk-86-64
        vtk-6.1.0-vc14-64
        ...
qtcanpool
    thirdparty
        opencascade
            opencascade.pri
            ...
```

其中，opencascade.pri 用于链接 opencascade 及其依赖库，默认 OPENCASCADE_ROOT 变量设置为上述 3rdparty/OpenCASCADE-7.7.0-vc14-64，如果 opencascade 安装在其它位置，请根据实际情况修改该变量。

# 使用

## occQt 示例
occQt 是一个简单的 OpenCASCADE Qt 示例

仓库地址：https://github.com/eryar/occQt

使用步骤：
- 下载 occQt 代码
- 拷贝 occQt 目录内所有文件到 qtcanpool/demos/thirdpary/occQt 目录中
- 进入 qtcanpool/demos/thirdpary/occQt 目录
- 替换 occQt.pro 为 patch/occQt.pro （使用 patch 目录中的 occQt.pro）
- 根据 patch/patch.diff 差异修改代码
- 构建 occQt
- 运行 occQt

备注：
- 目前 opencascade.pri 中的依赖库是根据 occQt 示例所需配置的，实际项目需要哪些根据实际情况配置即可。
- STATIC_FILES 变量用于配置 opencascade 及其依赖的库。依赖库需要先配置好，而 opencascade 的库是由 OPENCASCADE_LIBS 根据实际使用情况配置（70+ 个库并不一定全需要，所以按需配置）
