# HC32-MDK 项目

## 项目概述

这是基于 HC32F460 微控制器的开发项目，使用 CMake 构建系统。

## 构建要求

- ARM GCC 工具链（arm-none-eabi-gcc）
- CMake (>= 3.20)
- MinGW 或 MSYS2（Windows平台）

## 快速开始

### 编译项目

使用 `build.bat` 脚本编译项目：

```bash
# 默认使用Debug模式编译
build.bat

# 使用Release模式编译
build.bat Release
```

编译成功后，生成的文件（.elf, .hex, .bin）将位于 `build/output` 目录中。

### 清理项目

使用 `clean.bat` 脚本清理编译生成的文件：

```bash
clean.bat
```

## VSCode 与 clangd 集成

本项目已配置支持 clangd 代码智能提示和补全功能。

### 设置步骤

1. 安装 VSCode 插件：
   - clangd
   - CMake
   - CMake Tools

2. 首次编译项目以生成 `compile_commands.json` 文件：
   ```bash
   build.bat
   ```

3. VSCode 设置中添加以下配置：
   ```json
   {
     "clangd.arguments": [
       "--background-index",
       "--compile-commands-dir=${workspaceFolder}",
       "--header-insertion=never"
     ]
   }
   ```

4. 重启 VSCode 或重启 clangd 服务即可享受智能代码补全和导航。

## 相关文档

- [HC32F460 用户手册](https://www.hdsc.com.cn/Category83-1424)
- [ARM GCC 文档](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain)
- [CMake 文档](https://cmake.org/documentation/) 