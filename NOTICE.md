# 来源、署名与第三方许可

本项目基于 https://github.com/glKarin/weibomm ，上游基础提交为 `c767ff13d022e57a3b2ae5b7a22cd5b1dbeb492a`。

- 原始 MeeGo 微博客户端、微博品牌与相关内容：新浪微博及相应权利人
- 开源基础与此前改版：Karin · Weibo++ / Weibomm
- 2026 年可用性更新与新界面 UI 设计：Chih
- 开发协助：OpenAI Codex

## 代码许可

上游 `debian/copyright` 与 `qtc_packaging/debian_harmattan/copyright` 声明 GNU GPL v2 或更新版本，并仍含原始打包模板字段。本仓库保留这些原文，并提供 GPL v2 全文（根目录 `LICENSE`）。本次新增和修改的应用代码按 GPL-2.0-or-later 提供；已有文件的版权与许可声明继续适用。

这不是对所有资源的统一重新授权。图片、图标、表情、音频、微博名称、商标及微博服务内容归各自权利人，其授权范围不能仅从应用代码的许可证推定。

## 第三方组件

| 组件 | 用途／来源 | 许可 |
| --- | --- | --- |
| Mbed TLS 3.6.7 | 应用私有 TLS helper；随仓库提供对应上游源码压缩包 | Apache-2.0 OR GPL-2.0-or-later，见 `LICENSES/Mbed-TLS-3.6.7.txt` 和源码各文件 |
| core-js 2.6.12 | 旧网页引擎的 JavaScript 兼容层 | MIT，见 `LICENSES/core-js-2.6.12.txt` |
| QJson | `qjson/`，保留原作者和文件头 | LGPL v2 或更新版本，见各文件和 `LICENSES/LGPL-2.1.txt` |
| Qt Mobility 源码片段 | `src/qtm/`，保留 Nokia 文件头 | 文件头列明的 LGPL 2.1／Qt 例外或 GPL 3 等条款；相关文本见 `LICENSES/` |
| CA bundle | 运行时 `cacert.pem`，源文件含 Mozilla／curl 来源说明 | 保留 PEM 内版权与来源；不替换系统证书库 |

本项目与新浪微博无官方隶属关系，不代表新浪微博、Nokia 或 Karin 提供维护或背书。
