# 编译与打包

这是面向 **Qt 4.7.4 / Qt Quick 1.1 / MeeGo Harmattan** 的旧平台项目，不能用 Qt 6 或普通桌面 Qt 直接替代编译。

## 依赖

- 已配置的 Harmattan Qt SDK / MADDE 交叉编译工具链及对应 sysroot。
- Qt Declarative、Network、SQL、WebKit、XML、DBus。
- Qt Mobility multimedia / systeminfo / location，MeeGo Touch、videosuiteinterface 及 zlib、libcrypto 等原项目依赖（以 `weibomm.pro` 为准）。
- Node.js 18 或更新版本用于本仓库打包工具。
- 支持 bzip2 的 `tar`，用于解压随仓库提供的 Mbed TLS 3.6.7 源码。

本次开发使用的目标名为 `harmattan_10.2011.34-1_rt1.2`，编译器为 `arm-none-linux-gnueabi`。目标名随 SDK 安装情况可能不同。仓库不附带 Nokia SDK、系统镜像或商业工具链。

## 编译应用

以下 `qmake`、`make` 和 `strip` 均应使用 **Harmattan 目标工具链**，在已配置的 SDK shell 中执行。每个构建目录从空目录开始；修改模型头文件后建议 clean rebuild，以避免旧工具链的依赖追踪问题。

```sh
mkdir -p build build-app
cd build-app
qmake ../weibomm.pro
make -j4
arm-none-linux-gnueabi-strip -o ../build/weibo++ weibo++
cd ..
```

Windows MADDE 环境中，可使用 `mad -t <target> qmake ...`、`mad -t <target> make ...`；具体 SDK shell 入口与可执行文件路径由本机安装决定。

## 编译 TLS helper

先在普通主机终端校验并解压对应源码：

```sh
node tools/prepare-vendor.cjs
```

然后回到 Harmattan SDK shell：

```sh
mkdir -p build-tls
cd build-tls
qmake ../tls/tls.pro
make -j4
arm-none-linux-gnueabi-strip -o ../build/weibomm-tls weibomm-tls
cd ..
```

`tls/tls.c` 与发布版 helper 的源文件一致；`tls/tls.pro` 仅将 Mbed TLS 源码路径改成仓库内 `third_party/`。helper 采用应用私有目录，不替换系统 TLS 库，并保留证书和主机名校验。

## 生成安装包

在仓库根目录的普通 Node.js 环境执行：

```sh
node tools/package.cjs
```

生成 `dist/WeiboLite_1.0.0_armel.deb`。脚本只使用 Node.js 标准库，不依赖原始 `app.deb`、本机其他项目或手机连接。

`packaging/manifest.json` 记录发布版的安装路径、权限、源码映射和 SHA-256。`packaging/runtime/` 保存证书、兼容脚本及其他部署所需文件；应用源码、QML 和图像从仓库相应路径读取。现有 `debian/`、`qtc_packaging/` 保留上游历史打包资料；**1.0.0 的正式打包入口是 `tools/package.cjs`**。

若使用与正式版完全一致的两个二进制，可执行 `node tools/package.cjs --verify-release` 检查全部安装文件与 1.0.0 的映射和摘要一致。自行重编译后默认不要求二进制摘要相同；不同编译环境会产生不同二进制或压缩结果，不承诺安装包逐字节可重复。

发布的预编译安装包已经过 N9 真机测试。仓库打包工具另外验证了安装文件与正式安装包一致；没有对另一套全新 SDK 安装环境完成从零构建验证。
