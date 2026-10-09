# 包管理描述（vcpkg / Conan）

本目录与仓库根的 `conanfile.py` 提供 qtcanpool 3.0 的**包管理骨架**，用于把项目
通过 `find_package(QtCanpool)` 暴露给下游（见 [`../projects/consume`](../projects/consume)）。

| 文件 | 说明 |
| :--- | :--- |
| [`qtcanpool/vcpkg.json`](./qtcanpool/vcpkg.json) | vcpkg port 清单：版本 `3.0.0`，依赖 `qtbase` |
| [`qtcanpool/portfile.cmake`](./qtcanpool/portfile.cmake) | 从源码构建并安装 `QtCanpool` CMake 包 |
| [`../conanfile.py`](../conanfile.py) | Conan 2 配方骨架：`CMakeToolchain` / `CMakeDeps` |

> ⚠️ **两者均为骨架，未经 CI 验证**（vcpkg 的 `qtbase` 构建代价以小时计，不适合放进本项目的 CI）。
> 文档与骨架按"社区可贡献、作者未在 CI 验证"标注，避免给出"已完整支持"的错误信号。
> 这与 [`../doc/ROADMAP-3.0.md`](../doc/ROADMAP-3.0.md) 风险表"先出最简 port，社区可贡献"一致。

## 已注意到的安装约定

qtcanpool 把**公开头文件**与 **CMake 包配置**（`QtCanpoolConfig.cmake` / `QtCanpoolTargets.cmake`）
放进带 `EXCLUDE_FROM_ALL` 的 **`Devel` 组件**。因此一次普通的 `cmake --install` 只会装出运行库，
必须再执行一次组件安装：

```bash
cmake --install <build> --component Devel --prefix <prefix>
```

两个骨架都已据此处理（vcpkg 在 `portfile.cmake` 里显式安装 `Devel`；Conan 用
`cmake.install(component="Devel")`）。

## 使用（未验证，仅供参考）

```bash
# vcpkg（需先补全 portfile.cmake 里的 SHA512）
vcpkg install qtcanpool

# Conan 2
conan create . --build=missing
```
